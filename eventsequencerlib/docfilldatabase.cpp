#include "docfilldatabase.h"

#include <QDate>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

namespace {
QString databaseFileName()
{
    QString dbFolder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dbFolderDir(dbFolder);
    if (!dbFolderDir.exists()) {
        dbFolderDir.mkpath(".");
    }
    return dbFolder + "/docfill.sqlite3";
}

bool databaseInitConnection(QString* errorMessage)
{
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    if (!db.isValid()) {
        *errorMessage = "Unable to add QSQLITE database";
        return false;
    }
    db.setDatabaseName(databaseFileName());
    const auto ok = db.open();
    if (!ok) {
        *errorMessage = db.lastError().text();
        return false;
    }
    return true;
}

bool databaseInitTables(QString* errorMessage)
{
    QSqlDatabase db = QSqlDatabase::database();
    QStringList tables = db.tables();

    const int expectedVersion = 1;

    if (tables.contains("DocFill_DbVersion", Qt::CaseInsensitive)) {
        QSqlQuery query;
        bool ok = query.exec("SELECT Version FROM DocFill_DbVersion");
        if (ok) {
            if (!query.next()) {
                *errorMessage = QString("Unable to query database version: No rows");
                return false;
            }
            int version = query.value(0).toInt();
            if (version != expectedVersion) {
                *errorMessage = QString("Unsupported database version: %1 - expecting %2").arg(version).arg(expectedVersion);
                return false;
            }
        } else {
            *errorMessage = QString("Unable to query database version: %1").arg(query.lastError().text());
            return false;
        }
    } else {
        QSqlQuery query;
        bool ok = query.exec("CREATE TABLE DocFill_DbVersion (Version INTEGER)");
        if (!ok) {
            *errorMessage = QString("Unable to create version table: %1").arg(query.lastError().text());
            return false;
        }
        ok = query.exec(QString("INSERT INTO DocFill_DbVersion (Version) VALUES (%1)").arg(expectedVersion));
        if (!ok) {
            *errorMessage = QString("Unable to insert in version table: %1").arg(query.lastError().text());
            return false;
        }

        // DocFill_Stats
        {
            QSqlQuery query;
            bool ok = query.exec("CREATE TABLE `DocFill_Stats` ( `LocalYYYYMMDD` INTEGER, `Key` TEXT, `Value` NUMERIC DEFAULT 0, PRIMARY KEY(`LocalYYYYMMDD`,`Key`) )");
            if (!ok) {
                *errorMessage = QString("Unable to create stats table: %1").arg(query.lastError().text());
                return false;
            }
        }
    }

    return true;
}
} // namespace anonymous

QString DocFillDatabase::errorMessage() const
{
    return errorMessage_;
}

void DocFillDatabase::setErrorMessage(QString errorMessage)
{
    if (errorMessage_ != errorMessage) {
        errorMessage_ = errorMessage;
        emit errorMessageChanged();
    }
}

void DocFillDatabase::statsAddTodayAssignedDuration(qint64 duration)
{
    if (!initWasSuccessful_) {
        return;
    }
    setErrorMessage("");

    auto today = QDate::currentDate();
    int yyyymmdd = today.year() * 10000 + today.month() * 100 + today.day();

    QSqlQuery query;
    bool ok = query.prepare("INSERT OR REPLACE INTO `DocFill_Stats` (`LocalYYYYMMDD`, `Key`, `Value`) \n"
                            "VALUES (:LocalYYYYMMDD, :Key,                                            \n"
                            "         COALESCE((SELECT `Value` FROM `DocFill_Stats`                   \n"
                            "                  WHERE `LocalYYYYMMDD` = :LocalYYYYMMDD AND             \n"
                            "                        `Key` = :Key), 0) + :delta)                      \n");
    if (!ok) {
        setErrorMessage(QString("Stats: %1").arg(query.lastError().text()));
        return;
    }
    query.bindValue(":delta", duration);
    query.bindValue(":LocalYYYYMMDD", yyyymmdd);
    query.bindValue(":Key", "AssignedDuration");
    ok = query.exec();
    if (!ok) {
        setErrorMessage(QString("Stats: %1").arg(query.lastError().text()));
        return;
    }
}

void DocFillDatabase::init()
{
    // Called from constructor, so not emitting errorMessageChanged here

    if (!databaseInitConnection(&errorMessage_)) {
        return;
    }
    if (!databaseInitTables(&errorMessage_)) {
        return;
    }
    initWasSuccessful_ = true;
}

DocFillDatabase::DocFillDatabase(QObject *parent) : QObject(parent)
{
    init();
}

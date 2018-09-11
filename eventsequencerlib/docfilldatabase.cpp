#include "docfilldatabase.h"

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

    if (tables.contains("DocFill_DbVersion")) {
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
    }

    return true;
}
} // namespace anonymous

QString DocFillDatabase::errorMessage() const
{
    return errorMessage_;
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

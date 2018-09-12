#include "docfilldatabase.h"

#include "describeduration.h"

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

namespace {
static const char QUERY_ADD_STAT[] =
    "INSERT OR REPLACE INTO `DocFill_Stats` (`LocalYYYYMMDD`, `Key`, `Value`) \n"
    "VALUES (:LocalYYYYMMDD, :Key,                                            \n"
    "         COALESCE((SELECT `Value` FROM `DocFill_Stats`                   \n"
    "                  WHERE `LocalYYYYMMDD` = :LocalYYYYMMDD AND             \n"
    "                        `Key` = :Key), 0) + :delta)                      \n";
}

void DocFillDatabase::statsAddAssignedDuration(int yyyymmdd, qint64 duration)
{
    if (!initWasSuccessful_) {
        return;
    }
    setErrorMessage("");

    QSqlQuery query;
    bool ok = query.prepare(QUERY_ADD_STAT);
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

QString DocFillDatabase::statsGenerateReport()
{
    if (!initWasSuccessful_) {
        return QString("Database init failed: %1").arg(errorMessage_);
    }

    QSqlQuery query;
    bool ok = query.prepare("SELECT `LocalYYYYMMDD`, `Value` FROM `DocFill_Stats` \n"
                            "WHERE `Key` = :Key                                   \n"
                            "ORDER BY `LocalYYYYMMDD` DESC                        \n");
    if (!ok) {
        return QString("Error: %1").arg(query.lastError().text());
    }
    query.bindValue(":Key", "AssignedDuration");
    ok = query.exec();
    if (!ok) {
        return QString("Error: %1").arg(query.lastError().text());
    }

    QString result = "Assigned duration by date of recording\n\n";
    while (query.next()) {
        auto yyyymmdd = query.value(0).toInt();
        auto value = query.value(1).value<qint64>();
        result.append(QString("%1 %2\n").arg(yyyymmdd).arg(DescribeDuration::describeDuration(value / 1000 / 1000)));
    }
    return result;
}

void DocFillDatabase::statsReset()
{
    // Not implementing showing error to the user. It should be clear that something
    // failed if pressing the button does nothing.
    if (!initWasSuccessful_) {
        return;
    }

    QSqlQuery query;
    bool ok = query.prepare("DELETE FROM `DocFill_Stats` \n"
                            "WHERE `Key` = :Key          \n");
    if (!ok) {
        return;
    }
    query.bindValue(":Key", "AssignedDuration");
    ok = query.exec();
    if (!ok) {
        return;
    }
}

QString DocFillDatabase::statsMerge(std::map<int, qint64> &data)
{
    if (!initWasSuccessful_) {
        return QString("Database init failed: %1").arg(errorMessage_);
    }

    QSqlDatabase db = QSqlDatabase::database();

    QSqlQuery query(db);
    bool ok = query.prepare(QUERY_ADD_STAT);
    if (!ok) {
        return QString("Error: %1").arg(query.lastError().text());
    }

    QVariantList yyyymmdds;
    QVariantList durations;
    QVariantList assignedDurations;

    for (auto& pair : data) {
        yyyymmdds.push_back(pair.first);
        durations.push_back(pair.second);
        assignedDurations.push_back("AssignedDuration");
    }

    query.bindValue(":LocalYYYYMMDD", yyyymmdds);
    query.bindValue(":delta", durations);
    query.bindValue(":Key", assignedDurations);

    if (!db.transaction()) {
        return QString("Cannot begin transaction: %1").arg(db.lastError().text());
    }

    if (!query.execBatch()) {
        QString errorMsg = query.lastError().text();
        db.rollback();
        return QString("Error: %1").arg(errorMsg);
    }

    if (!db.commit()) {
        return QString("Cannot commit transaction: %1").arg(db.lastError().text());
    }

    return "";
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

#ifndef SAFERFILEREPLACEMENT_H
#define SAFERFILEREPLACEMENT_H

#include <QFile>

class SaferFileReplacement {
public:
    static const QString BACKUP_FILE_SUFFIX;
private:

    QString fileName_;
    QFile file_;

    bool writingTmpFile_ = false;
    QString tmpFileName_;

public:
    enum class BackupResult {
        NotNeeded,
        Success,
        Failure
    };
private:
    BackupResult backupResult_ = BackupResult::NotNeeded;

public:
    SaferFileReplacement(const QString& fileName);
    QFile& file();
    BackupResult backupResult();
    void begin();
    bool commit();
    void rollback();
    ~SaferFileReplacement();
};

#endif // SAFERFILEREPLACEMENT_H

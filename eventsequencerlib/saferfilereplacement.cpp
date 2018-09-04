#include "saferfilereplacement.h"

const QString SaferFileReplacement::BACKUP_FILE_SUFFIX("~");

SaferFileReplacement::SaferFileReplacement(const QString &fileName) : fileName_(fileName)
{

}

QFile &SaferFileReplacement::file()
{
    return file_;
}

SaferFileReplacement::BackupResult SaferFileReplacement::backupResult()
{
    return backupResult_;
}

void SaferFileReplacement::begin()
{
    if (QFile::exists(fileName_)) {
        tmpFileName_ = fileName_ + ".tmp"; // TODO Race condition here. Either prompt the user for overwrite, or pick a new name.
        file_.setFileName(tmpFileName_);
        writingTmpFile_ = true;
    } else {
        writingTmpFile_ = false;
        file_.setFileName(fileName_);
    }
    backupResult_ = BackupResult::NotNeeded;
}

bool SaferFileReplacement::commit()
{
    if (writingTmpFile_) {
        {
            QFile target(fileName_);
            if (target.exists()) {
                QString backupFileName = fileName_ + "~";
                QFile::remove(backupFileName);
                if (target.rename(backupFileName)) {
                    backupResult_ = BackupResult::Success;
                } else {
                    backupResult_ = BackupResult::Failure;

                    // We'd rather not have a backup file than not be able
                    // to save. We have a good version of the data in
                    // the temp file.
                    target.remove();
                }
            } else {
                // Race condition
                backupResult_ = BackupResult::NotNeeded;
            }
        }

        if (file_.rename(fileName_)) {
            writingTmpFile_ = false;
            return true;
        } else {
            rollback();
            return false;
        }

    } else {
        backupResult_ = BackupResult::NotNeeded;
        return true;
    }
}

void SaferFileReplacement::rollback()
{
    if (writingTmpFile_) {
        file_.close();
        file_.remove();
        writingTmpFile_ = false;
    }
}

SaferFileReplacement::~SaferFileReplacement() {
    if (writingTmpFile_) {
        rollback();
    }
}

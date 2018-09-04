#include "documentmanagerdeleter.h"

#include "documentpaths.h"

#include <QFileInfo>
#include <QDir>

QString DocumentManagerDeleter::filePath() const
{
    return filePath_;
}

void DocumentManagerDeleter::setFilePath(const QString &filePath)
{
    if (filePath_ != filePath) {
        filePath_ = filePath;
        emit filePathChanged();
        updateQueuedForDeletion();
    }
}

QVariantList DocumentManagerDeleter::queuedForDeletion() const
{
    return queuedForDeletion_;
}

QString DocumentManagerDeleter::actuallyDelete()
{
    for (QVariant& var : queuedForDeletion_) {
        auto dpr = var.value<DocumentPathsResponse>();
        if (dpr.isDirectory) {
            QDir dir(dpr.filePath);
            if (!dir.removeRecursively()) {
                // TODO Stop using removeRecursively() because it doesn't give error msg.
                return QString("Cannot recursively remove directory: %1")
                    .arg(dpr.filePath);
            }
        } else {
            QFile file(dpr.filePath);
            if (!file.remove()) {
                return QString("Cannot remove %1: %2")
                    .arg(dpr.filePath)
                    .arg(file.errorString());
            }
        }
    }
    return "Success";
}

void DocumentManagerDeleter::updateQueuedForDeletion()
{
    queuedForDeletion_.clear();
    if (filePath_.isEmpty()) {
        emit queuedForDeletionChanged();
        return;
    }
    QFileInfo fi(filePath_);
    if (!fi.exists()) {
        emit queuedForDeletionChanged();
        return;
    }

    for (auto request :
         {// These are ordered such that the less important stuff is
          // deleted first. If delete fails, we won't continue
          // further.
          DocumentPaths::PathRequest::JSON_EXPORT,
          DocumentPaths::PathRequest::PLAYTOFILE_EXPORT,
          DocumentPaths::PathRequest::DOCUMENT_BACKUP,
          DocumentPaths::PathRequest::DATA_DIRECTORY,
          DocumentPaths::PathRequest::DOCUMENT}) {
        DocumentPathsResponse response;
        DocumentPaths::pathQuery(filePath_, request, &response);

        if (QFile::exists(response.filePath)) {
            queuedForDeletion_.push_back(QVariant::fromValue(response));
        }
    }

    emit queuedForDeletionChanged();
}

DocumentManagerDeleter::DocumentManagerDeleter(QObject *parent) : QObject(parent)
{

}

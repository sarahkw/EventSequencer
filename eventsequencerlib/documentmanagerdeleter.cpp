#include "documentmanagerdeleter.h"

#include "documentpaths.h"

#include <QFileInfo>

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
    return "Not implemented";
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

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

    for (auto request : {
             DocumentPaths::PathRequest::DOCUMENT,
             DocumentPaths::PathRequest::DOCUMENT_BACKUP,
             DocumentPaths::PathRequest::DATA_DIRECTORY,
             DocumentPaths::PathRequest::JSON_EXPORT,
             DocumentPaths::PathRequest::PLAYTOFILE_EXPORT}) {
        DocumentPaths::PathResponse response;
        DocumentPaths::pathQuery(filePath_, request, &response);
        queuedForDeletion_.push_back(response.filePath);
    }

    emit queuedForDeletionChanged();
}

DocumentManagerDeleter::DocumentManagerDeleter(QObject *parent) : QObject(parent)
{

}

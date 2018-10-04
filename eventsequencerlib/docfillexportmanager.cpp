#include "docfillexportmanager.h"

#include <QDir>

#include <memory>

Document *DocFillExportManager::document() const
{
    return document_;
}

void DocFillExportManager::setDocument(Document *document)
{
    if (document_ != document) {
        if (document_ != nullptr) {
            document_->disconnect(this);
        }
        document_ = document;
        if (document_ != nullptr) {
            QObject::connect(document_, &QObject::destroyed,
                             this, &DocFillExportManager::clearDocument);
        }
        emit documentChanged();
        updateDefaultOutputPaths();
    }
}

void DocFillExportManager::clearDocument()
{
    document_ = nullptr;
    emit documentChanged();
    updateDefaultOutputPaths();
}

QString DocFillExportManager::defaultExportJsonOutputPath() const
{
    return defaultExportJsonOutputPath_;
}

QString DocFillExportManager::defaultPlayToFileOutputPath() const
{
    return defaultPlayToFileOutputPath_;
}

QString DocFillExportManager::defaultExportHtmlOutputPath() const
{
    return defaultExportHtmlOutputPath_;
}

QString DocFillExportManager::requestExportJson(Document* document)
{
    return batchService_.requestExportJson(document->currentUrl());
}

QString DocFillExportManager::requestExportPlayToFile(Document* document)
{
    return batchService_.requestExportPlayToFile(document->currentUrl());
}

QString DocFillExportManager::requestExportHtml(Document* document)
{
    return batchService_.requestExportHtml(document->currentUrl());
}

void DocFillExportManager::requestCancelWorker()
{
    batchService_.requestCancelWorker();
}

void DocFillExportManager::requestClearStatus()
{
    batchService_.requestClearStatus();
}

void DocFillExportManager::tempUpdateDefaultOutputPaths()
{
    updateDefaultOutputPaths();
}

bool DocFillExportManager::defaultExportJsonOutputPathExists() const
{
    return defaultExportJsonOutputPathExists_;
}

bool DocFillExportManager::defaultPlayToFileOutputPathExists() const
{
    return defaultPlayToFileOutputPathExists_;
}

bool DocFillExportManager::defaultExportHtmlOutputPathExists() const
{
    return defaultExportHtmlOutputPathExists_;
}

bool DocFillExportManager::deleteDefaultExportJsonOutputPath()
{
    const bool success = QFile::remove(defaultExportJsonOutputPath_);
    updateDefaultOutputPaths();
    return success;
}

bool DocFillExportManager::deleteDefaultPlayToFileOutputPath()
{
    const bool success = QFile::remove(defaultPlayToFileOutputPath_);
    updateDefaultOutputPaths();
    return success;
}

bool DocFillExportManager::deleteDefaultExportHtmlOutputPath()
{
    if (defaultExportHtmlOutputPath_.size() < 5) {
        // I don't know what would happen if this is blank somehow, but this
        // is for safety. Choosing 5 just so it can't be like "/" or "C:/".
        Q_ASSERT(false);
        return false;
    }
    QDir dir(defaultExportHtmlOutputPath_);
    const bool success = dir.exists() && dir.removeRecursively();
    updateDefaultOutputPaths();
    return success;
}

void DocFillExportManager::updateDefaultOutputPaths()
{
    defaultExportJsonOutputPath_.clear();
    defaultPlayToFileOutputPath_.clear();
    defaultExportHtmlOutputPath_.clear();
    defaultExportJsonOutputPathExists_ = false;
    defaultPlayToFileOutputPathExists_ = false;
    defaultExportHtmlOutputPathExists_ = false;
    if (document_ != nullptr) {
        DocumentPathsResponse pathResponse;
        if (document_->pathQuery(DocumentPaths::PathRequest::JSON_EXPORT, &pathResponse)) {
            defaultExportJsonOutputPath_ = pathResponse.filePath;
            defaultExportJsonOutputPathExists_ = QFile::exists(defaultExportJsonOutputPath_);
        }
        if (document_->pathQuery(DocumentPaths::PathRequest::PLAYTOFILE_EXPORT, &pathResponse)) {
            defaultPlayToFileOutputPath_ = pathResponse.filePath;
            defaultPlayToFileOutputPathExists_ = QFile::exists(defaultPlayToFileOutputPath_);
        }
        if (document_->pathQuery(DocumentPaths::PathRequest::HTML_EXPORT, &pathResponse)) {
            defaultExportHtmlOutputPath_ = pathResponse.filePath;
            defaultExportHtmlOutputPathExists_ = QFile::exists(defaultExportHtmlOutputPath_);
        }
    }
    emit defaultOutputPathsChanged();
}

DocFillExportManager::DocFillExportManager(QObject *parent) : QObject(parent)
{
    QObject::connect(&batchService_, &BatchServiceFactory::Type::statusChanged,
                     this, &DocFillExportManager::batchServiceStatusChanged);
}

QVariant DocFillExportManager::batchServiceStatus()
{
    return batchService_.status();
}

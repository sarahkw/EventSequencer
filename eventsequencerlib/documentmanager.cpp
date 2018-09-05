#include "documentmanager.h"

#include <QDir>

QString DocumentManager::documentsPath() const
{
    return documentsPath_;
}

void DocumentManager::setDocumentsPath(const QString &documentsPath)
{
    if (documentsPath_ != documentsPath) {
        documentsPath_ = documentsPath;
        emit documentsPathChanged();

        updateItems();
    }
}

QVariantList DocumentManager::items() const
{
    return items_;
}

void DocumentManager::refresh()
{
    updateItems();
}

void DocumentManager::updateItems()
{
    items_.clear();

    if (!documentsPath_.isEmpty()) {
        emit beforeScanDocuments();
    }

    QDir dir(documentsPath_);
    for (QFileInfo& fi : dir.entryInfoList({"*.evseq"}, QDir::Files, QDir::Name)) {
        DocumentManagerItem dmi;
        dmi.displayName_ = fi.completeBaseName();
        dmi.filePath_ = fi.filePath();
        items_.push_back(QVariant::fromValue(dmi));
    }

    emit itemsChanged();
}

DocumentManager::DocumentManager(QObject *parent) : QObject(parent)
{

}

QString DocumentManagerItem::filePath() const
{
    return filePath_;
}

QString DocumentManagerItem::displayName() const
{
    return displayName_;
}

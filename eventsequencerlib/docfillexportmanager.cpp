#include "docfillexportmanager.h"

#include "document.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <QDebug> // FOR DEV WORK

QObject *DocFillExportManager::document() const
{
    return document_;
}

void DocFillExportManager::setDocument(QObject *document)
{
    Document* castDocument = qobject_cast<Document*>(document);

    if (document_ != castDocument) {
        if (document_ != nullptr) {
            document_->disconnect(this);
        }
        document_ = castDocument;
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

void DocFillExportManager::exportJson(channel::ChannelBase* textChannel,
                                      channel::ChannelBase* resourceChannel,
                                      QString outputPath)
{
    if (document_ == nullptr || textChannel == nullptr || resourceChannel == nullptr) {
        qWarning("Cannot export; no document or missing channel");
        return;
    }

    QJsonObject obj;
    obj["a"] = true;

    QJsonDocument jsonDoc(obj);
    qInfo() << jsonDoc.toJson().data();
}

void DocFillExportManager::updateDefaultOutputPaths()
{
    defaultExportJsonOutputPath_.clear();
    defaultPlayToFileOutputPath_.clear();
    if (document_ != nullptr) {
        auto pathGen = document_->exportPathGenerator();
        if (!!pathGen) {
            defaultExportJsonOutputPath_ = pathGen(".json");
            defaultPlayToFileOutputPath_ = pathGen(".au");
        }
    }
    emit defaultOutputPathsChanged();
}

DocFillExportManager::DocFillExportManager(QObject *parent) : QObject(parent)
{

}

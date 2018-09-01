#include "docfillexportmanager.h"

#include "document.h"
#include "channel/textchannel.h"
#include "channel/collatechannel.h"
#include "managedresources.h"

#include <QJsonArray>
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

    auto* castTextChannel = qobject_cast<channel::TextChannel*>(textChannel);
    if (castTextChannel == nullptr) {
        qWarning("Unsupported text channel type");
        return;
    }

    auto* castResourceChannel = qobject_cast<channel::CollateChannel*>(resourceChannel);
    if (castResourceChannel == nullptr) {
        qWarning("Unsupported resource channel type");
        return;
    }

    QString content = castTextChannel->content();

    QJsonArray jsonSegments;

    for (auto& segment : castResourceChannel->segments()) {
        QJsonObject obj;
        Strip* s = segment.strip;
        obj["startPosition"] = segment.segmentStart;
        obj["length"] = segment.segmentLength;
        obj["text"] = content.mid(segment.segmentStart, segment.segmentLength);
        if (segment.segmentType == channel::CollateChannel::SegmentType::Conflict) {
            obj["hasConflict"] = true;
        }
        if (s != nullptr) {
            QUrl url = s->resourceUrl();
            if (!url.isEmpty()) {
                obj["file"] = ManagedResources::urlConvertToFileName(url.toString());
            }
        }
        jsonSegments.push_back(obj);
    }

    QJsonDocument jsonDoc(jsonSegments);
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

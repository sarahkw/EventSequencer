#include "docfillexportmanager.h"

#include "document.h"
#include "channel/textchannel.h"
#include "channel/collatechannel.h"
#include "managedresources.h"
#include "aufileheader.h"
#include "audioformatholder.h"
#include "resourcemetadata.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <memory>

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

QString DocFillExportManager::exportJson(channel::ChannelBase* textChannel,
                                         channel::ChannelBase* resourceChannel)
{
    if (textChannel == nullptr || resourceChannel == nullptr) {
        qWarning("Missing channel(s)");
        return "Internal error";
    }

    auto* castTextChannel = qobject_cast<channel::TextChannel*>(textChannel);
    if (castTextChannel == nullptr) {
        qWarning("Unsupported text channel type");
        return "Internal error";
    }

    auto* castResourceChannel = qobject_cast<channel::CollateChannel*>(resourceChannel);
    if (castResourceChannel == nullptr) {
        qWarning("Unsupported resource channel type");
        return "Internal error";
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

                // Let's make an attempt to read meta-data. Failures are silently
                // ignored because files may not have meta-data, may be missing,
                // etc.
                {
                    QString filePath;
                    ManagedResources mr(document_->fileResourceDirectory());
                    if (mr.urlConvertToFilePath(url, &filePath)) {
                        std::string createTime;
                        if (ResourceMetaData::readFromFile(filePath, &createTime)) {
                            obj["fileCreateTime"] = QString::fromStdString(createTime);
                        }
                    }
                    // RAII to clean up
                }
            }
        }
        jsonSegments.push_back(obj);
    }

    QFile file(defaultExportJsonOutputPath_);
    if (!file.open(QFile::NewOnly)) {
        return QString("Cannot open file: %1").arg(file.errorString());
    }

    QString error = "Success";

    QJsonDocument jsonDoc(jsonSegments);
    QByteArray toWrite = jsonDoc.toJson();
    qint64 written = file.write(toWrite);
    if (written != toWrite.size()) {
        error = QString("Cannot completely write file: %1").arg(file.errorString());
    } else if (!file.flush()) {
        error = QString("Cannot flush: %1").arg(file.errorString());
    } else {
        file.close();
    }

    updateDefaultOutputPaths();
    return error;
}

QString DocFillExportManager::exportPlayToFile(playable::PlayableBase *playable)
{
    if (playable == nullptr || document_ == nullptr || !document_->audioFormatHolderSet()) {
        return "Internal error";
    }

    if (!playable->isFinite()) {
        return "Cannot export from infinite audio input";
    }

    auto* audioFormatHolder =
        qobject_cast<AudioFormatHolder*>(document_->audioFormatHolderQObject());
    if (audioFormatHolder == nullptr) {
        return "Internal error";
    }
    auto audioFormat = audioFormatHolder->toQAudioFormat();

    // .au's have big endian samples
    audioFormat.setByteOrder(QAudioFormat::BigEndian);

    std::unique_ptr<QIODevice> source{playable->createPlayableDevice(audioFormat)};
    if (!source) {
        return playable->error();
    }
    source->open(QIODevice::ReadOnly);

    QFile file(defaultPlayToFileOutputPath_);
    if (!file.open(QFile::NewOnly)) {
        return QString("Cannot open file: %1").arg(file.errorString());
    }

    qint64 totalBytes = 0;
    QString error = "Success";

    AuFileHeader afh;
    if (!afh.loadFormat(audioFormat)) {
        qCritical("??? Format was good but now is not?");
        error = "Internal error";
        goto done;
    }

    if (!afh.writeFile(file)) {
        error = QString("Cannot write header: %1").arg(file.errorString());
        goto done;
    }

    char buffer[4096];
    for (;;) {
        qint64 gotBytes = source->read(buffer, sizeof(buffer));

        if (gotBytes > 0) {
            totalBytes += gotBytes;
            if (gotBytes != file.write(buffer, gotBytes)) {
                error = "Incomplete write";
                goto done;
            }
        } else if (gotBytes < 0) {
            error = "Read error";
            goto done;
        } else {
            // EOF
            break;
        }
    }

    if (!file.flush()) {
        error = QString("Cannot flush: %1").arg(file.errorString());
        goto done;
    }

    file.close();

    if (totalBytes == 0) {
        error = "Success, but the audio file has no data";
    }

done:
    updateDefaultOutputPaths();
    return error;
}

bool DocFillExportManager::defaultExportJsonOutputPathExists() const
{
    return defaultExportJsonOutputPathExists_;
}

bool DocFillExportManager::defaultPlayToFileOutputPathExists() const
{
    return defaultPlayToFileOutputPathExists_;
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

void DocFillExportManager::updateDefaultOutputPaths()
{
    defaultExportJsonOutputPath_.clear();
    defaultPlayToFileOutputPath_.clear();
    defaultExportJsonOutputPathExists_ = false;
    defaultPlayToFileOutputPathExists_ = false;
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
    }
    emit defaultOutputPathsChanged();
}

DocFillExportManager::DocFillExportManager(QObject *parent) : QObject(parent)
{

}

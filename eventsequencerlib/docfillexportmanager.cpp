#include "docfillexportmanager.h"

#include "document.h"
#include "docfillstructure.h"
#include "channel/textchannel.h"
#include "channel/collatechannel.h"
#include "managedresources.h"
#include "aufileheader.h"
#include "audioformatholder.h"
#include "resourcemetadata.h"
#include "playable/stripslist.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

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

QString DocFillExportManager::defaultExportHtmlOutputPath() const
{
    return defaultExportHtmlOutputPath_;
}

QString DocFillExportManager::exportJson(Document* document, QString outputPath)
{
    DocFillStructure dfstructure;
    if (!dfstructure.load(*document)) {
        return "Internal error";
    }

    QString content = dfstructure.textChannel->content();

    QJsonArray jsonSegments;

    for (auto& segment : dfstructure.collateChannel->segments()) {
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
                    ManagedResources mr(document->fileResourceDirectory());
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

    QFile file(outputPath);
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

    //updateDefaultOutputPaths();
    return error;
}

QString DocFillExportManager::exportPlayToFile(Document* document, QString outputPath)
{
    DocFillStructure dfstructure;
    if (!dfstructure.load(*document)) {
        return "Internal error";
    }

    playable::StripsList playable;
    playable.setFileResourceDirectory(document->fileResourceDirectory());
    playable.setSelectedChannel(dfstructure.collateChannel);
    playable.setSelectionMode(playable::StripsList::SelectionMode::ChannelFromBegin);
    
    AudioFormatHolder* audioFormatHolder = nullptr;
    if (document->audioFormatHolderSet()) {
        audioFormatHolder =
            qobject_cast<AudioFormatHolder*>(document->audioFormatHolderQObject());
    }
    if (audioFormatHolder == nullptr) {
        return "Internal error";
    }
    auto audioFormat = audioFormatHolder->toQAudioFormat();

    // .au's have big endian samples
    audioFormat.setByteOrder(QAudioFormat::BigEndian);

    std::unique_ptr<QIODevice> source{playable.createPlayableDevice(audioFormat)};
    if (!source) {
        return playable.error();
    }
    source->open(QIODevice::ReadOnly);

    QFile file(outputPath);
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
    //updateDefaultOutputPaths();
    return error;
}

QString DocFillExportManager::exportHtml(Document* document, QString outputPath)
{
    QDir dir(outputPath);
    if (dir.exists()) {
        return "Already exists!";
    }

//    if (!dir.mkpath(".")) {
//        return "Cannot create directory";
//    }

    //batchService_.requestExportHtml();
    //updateDefaultOutputPaths();
    return "";
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

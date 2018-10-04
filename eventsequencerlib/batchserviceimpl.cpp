#include "batchserviceimpl.h"

#include "audioformatholder.h"
#include "batchservicestatus.h"
#include "document.h"
#include "docfillstructure.h"
#include "managedresources.h"
#include "resourcemetadata.h"
#include "channel/textchannel.h"
#include "channel/collatechannel.h"
#include "playable/stripslist.h"
#include "endianmodifyingiodevice.h"
#include "supportedaudioformat.h"
#include "entireunitreader.h"

#include <QDir>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <lame/lame.h>

#include <cmath>

BatchServiceImplThread::BatchServiceImplThread(QUrl initialUrl)
{
    fileName_ = QFileInfo(initialUrl.toLocalFile()).completeBaseName();
}

void BatchServiceImplThread::run()
{
    auto result = process();
    if (result.success) {
        if (result.message.isEmpty()) {
            emit statusTextChanged(fileName_, QString("Success"));
        } else {
            emit statusTextChanged(fileName_, QString("Success: %1").arg(result.message));
        }
    } else {
        emit statusTextChanged(fileName_, QString("Failure: %1").arg(result.message));
    }
}

void BatchServiceImplThread::setFileNameFromPath(QString filePath)
{
    fileName_ = QFileInfo(filePath).fileName();
}

void BatchServiceImplThread::reportStatus(QString status)
{
    emit statusTextChanged(fileName_, status);
}

namespace {

class ExportJsonWorkerThread : public BatchServiceImplThread {
    QUrl documentUrl_;
public:
    ExportJsonWorkerThread(QUrl documentUrl) : BatchServiceImplThread(documentUrl), documentUrl_(documentUrl) {}
protected:
    FinalStatus process() override;
};

BatchServiceImplThread::FinalStatus ExportJsonWorkerThread::process()
{
    Document document;
    {
        auto result = document.load(documentUrl_);
        if (!result[0].toBool()) {
            return {false, QString("Cannot open: %1").arg(result[1].toString())};
        }
    }
    QString outputPath;
    {
        DocumentPathsResponse pathResponse;
        if (document.pathQuery(DocumentPaths::PathRequest::JSON_EXPORT, &pathResponse)) {
            outputPath = pathResponse.filePath;
        } else {
            return {false, "Internal error"};
        }
    }
    setFileNameFromPath(outputPath);

    DocFillStructure dfstructure;
    if (!dfstructure.load(document)) {
        return {false, "Internal error"};
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
                    ManagedResources mr(document.fileResourceDirectory());
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
        return {false, QString("Cannot open file: %1").arg(file.errorString())};
    }

    QString error = "Success";

    QJsonDocument jsonDoc(jsonSegments);
    QByteArray toWrite = jsonDoc.toJson();
    qint64 written = file.write(toWrite);
    if (written != toWrite.size()) {
        return {false, QString("Cannot completely write file: %1").arg(file.errorString())};
    } else if (!file.flush()) {
        return {false, QString("Cannot flush: %1").arg(file.errorString())};
    } else {
        file.close();
    }

    return {true, ""};
}

class ExportPlayToFileWorkerThread : public BatchServiceImplThread {
    QUrl documentUrl_;
public:
    ExportPlayToFileWorkerThread(QUrl documentUrl) : BatchServiceImplThread(documentUrl), documentUrl_(documentUrl) {}
protected:
    FinalStatus process() override;
};

BatchServiceImplThread::FinalStatus ExportPlayToFileWorkerThread::process()
{
    Document document;
    {
        auto result = document.load(documentUrl_);
        if (!result[0].toBool()) {
            return {false, QString("Cannot open: %1").arg(result[1].toString())};
        }
    }
    QString outputPath;
    {
        DocumentPathsResponse pathResponse;
        if (document.pathQuery(DocumentPaths::PathRequest::PLAYTOFILE_EXPORT, &pathResponse)) {
            outputPath = pathResponse.filePath;
        } else {
            return {false, "Internal error"};
        }
    }
    setFileNameFromPath(outputPath);

    DocFillStructure dfstructure;
    if (!dfstructure.load(document)) {
        return {false, "Internal error"};
    }

    playable::StripsList playable;
    playable.setFileResourceDirectory(document.fileResourceDirectory());
    playable.setSelectedChannel(dfstructure.collateChannel);
    playable.setSelectionMode(playable::StripsList::SelectionMode::ChannelFromBegin);

    AudioFormatHolder* audioFormatHolder = nullptr;
    if (document.audioFormatHolderSet()) {
        audioFormatHolder =
            qobject_cast<AudioFormatHolder*>(document.audioFormatHolderQObject());
    }
    if (audioFormatHolder == nullptr) {
        return {false, "Internal error"};
    }
    auto audioFormat = audioFormatHolder->toQAudioFormat();

    // .au's have big endian samples
    audioFormat.setByteOrder(QAudioFormat::BigEndian);

    std::unique_ptr<QIODevice> source{playable.createPlayableDevice(audioFormat)};
    if (!source) {
        return {false, playable.error()};
    }
    source->open(QIODevice::ReadOnly);

    QFile file(outputPath);
    if (!file.open(QFile::NewOnly)) {
        return {false, QString("Cannot open file: %1").arg(file.errorString())};
    }

    qint64 totalBytes = 0;

    AuFileHeader afh;
    if (!afh.loadFormat(audioFormat)) {
        qCritical("??? Format was good but now is not?");
        return {false, "Internal error"};
    }

    if (!afh.writeFile(file)) {
        return {false, QString("Cannot write header: %1").arg(file.errorString())};
    }

    char buffer[4096];
    for (;;) {
        qint64 gotBytes = source->read(buffer, sizeof(buffer));

        if (gotBytes > 0) {
            totalBytes += gotBytes;
            if (gotBytes != file.write(buffer, gotBytes)) {
                return {false, "Incomplete write"};
            }
        } else if (gotBytes < 0) {
            return {false, "Read error"};
        } else {
            // EOF
            break;
        }
    }

    if (!file.flush()) {
        return {false, QString("Cannot flush: %1").arg(file.errorString())};
    }

    file.close();

    return {true, ""};
}

class ExportHtmlWorkerThread : public BatchServiceImplThread {
    QUrl documentUrl_;
public:
    ExportHtmlWorkerThread(QUrl documentUrl) : BatchServiceImplThread(documentUrl), documentUrl_(documentUrl) {}
protected:
    FinalStatus process() override;
};

BatchServiceImplThread::FinalStatus ExportHtmlWorkerThread::process()
{
    Document document;
    {
        auto result = document.load(documentUrl_);
        if (!result[0].toBool()) {
            return {false, QString("Cannot open: %1").arg(result[1].toString())};
        }
    }
    QString outputPath;
    {
        DocumentPathsResponse pathResponse;
        if (document.pathQuery(DocumentPaths::PathRequest::HTML_EXPORT, &pathResponse)) {
            outputPath = pathResponse.filePath;
        } else {
            return {false, "Internal error"};
        }
    }
    setFileNameFromPath(outputPath);

    DocFillStructure dfstructure;
    if (!dfstructure.load(document)) {
        return {false, "Internal error"};
    }

    QDir dir(outputPath);
    if (dir.exists()) {
        return {false, "Already exists!"};
    }
    if (!dir.mkpath(".")) {
        return {false, "Cannot create directory"};
    }

    AudioFormatHolder* audioFormatHolder = nullptr;
    if (document.audioFormatHolderSet()) {
        audioFormatHolder =
            qobject_cast<AudioFormatHolder*>(document.audioFormatHolderQObject());
    }
    if (audioFormatHolder == nullptr) {
        return {false, "Internal error"};
    }
    auto audioFormat = audioFormatHolder->toQAudioFormat();
    if (!audioFormat.isValid()) {
        return {false, "Internal error"};
    }
    if (audioFormat.channelCount() != 1 && audioFormat.channelCount() != 2) {
        return {false, "Only mono and stereo supported"};
    }

    // I think LAME expects the byte order of the device.
    static_assert (static_cast<QAudioFormat::Endian>(QSysInfo::BigEndian) == QAudioFormat::BigEndian, "");
    static_assert (static_cast<QAudioFormat::Endian>(QSysInfo::LittleEndian) == QAudioFormat::LittleEndian, "");
    audioFormat.setByteOrder(static_cast<QAudioFormat::Endian>(QSysInfo::ByteOrder));

    auto allSegments = dfstructure.collateChannel->segments();
    std::vector<channel::CollateChannel::Segment> processableSegments;
    for (auto& s : allSegments) {
        if (s.strip != nullptr) {
            if (!s.strip->resourceUrl().isEmpty()) {
                processableSegments.push_back(s);
            }
        }
    }

    const auto digitCount = QString::number(processableSegments.size()).length();

    for (size_t i = 0; i < processableSegments.size(); ++i) {
        Strip* s = processableSegments[i].strip;
        reportStatus(QString("%1 of %2").arg(i).arg(processableSegments.size()));

        std::unique_ptr<SampleModifyingIODevice> resource;
        QString outputFilePath;
        {
            EndianModifyingIODevice* emiod{};
            QString errorMessage;
            if (playable::PlayableBase::buildEmiod(
                    document.fileResourceDirectory(), s->resourceUrl(),
                    audioFormat, &emiod, &errorMessage)) {
                resource.reset(emiod);
            } else {
                return {false, QString("Error: %1: %2").arg(s->resourceUrl().toString()).arg(errorMessage)};
            }

            outputFilePath = QString("%1/%2.mp3").arg(outputPath).arg(i, digitCount, 10, QChar('0'));
        }

        {
            const bool ok = resource->open(QIODevice::ReadOnly);
            Q_ASSERT(ok); // Can't happen, but just make sure.
        }

        QFile writeFile(outputFilePath);
        if (!writeFile.open(QFile::WriteOnly)) {
            return {false, QString("Error: %1: %2").arg(outputFilePath).arg(writeFile.errorString())};
        }

        auto lameCloser = [](lame_global_flags* gfp) {
            lame_close(gfp);
        };
        std::unique_ptr<lame_global_flags, decltype(lameCloser)> gfp(lame_init(), lameCloser);
        Q_ASSERT(!!gfp); // TODO error handling

        lame_set_in_samplerate(&*gfp, audioFormat.sampleRate());
        if (audioFormat.channelCount() == 1 || audioFormat.channelCount() == 2) {
            lame_set_num_channels(&*gfp, audioFormat.channelCount());
        } else {
            // I guess if we wanted to support more channels, we could just take
            // the first 2 channels. But I don't think anyone will do that.
            return {false, QString("Can only encode with 1 or 2 channels")};
        }
        {
            int rcode = lame_init_params(&*gfp);
            if (rcode < 0) {
                return {false, QString("Error: %1: LAME Error: %2").arg(s->resourceUrl().toString()).arg(rcode)};
            }
        }

        Q_ASSERT(audioFormat.sampleSize() > 0);
        const size_t bytesPerSample = size_t(audioFormat.sampleSize()) / 8;
        const size_t bufferSize = 16384; // QIODEVICE_BUFFERSIZE, but that's in a private header.
        Q_ASSERT(audioFormat.channelCount() > 0);
        const size_t frameCount = bufferSize / bytesPerSample / size_t(audioFormat.channelCount());
        const size_t sampleCount = frameCount * size_t(audioFormat.channelCount());

        // "worst case" from docs. I add ceil just to be safe, since the doc isn't clear.
        const auto mp3buffer_size = size_t(std::ceil(1.25 * sampleCount + 7200));

        std::vector<unsigned char> mp3buffer(mp3buffer_size);
        std::vector<char> tmpbuf(bufferSize); // For readSamples

        switch (SupportedAudioFormat::classify(audioFormat)) {
        case SupportedAudioFormat::Type::NotSupported:
            return {false, QString("Unsupported audio format for encoding")};
        case SupportedAudioFormat::Type::Float:
            break;
        case SupportedAudioFormat::Type::Short: {
            std::vector<short> samples(sampleCount);

            for (;;) {
                EntireUnitReader::Error error{};

                size_t framesRead = EntireUnitReader::readUnitGroups<short>(
                    [&resource](char* data, qint64 maxlen) -> qint64 {
                        return resource->read(data, maxlen);
                    },
                    samples.data(), samples.size(), frameCount,
                    size_t(audioFormat.channelCount()), &error);

                if (error & EntireUnitReader::NegativeOneError) {
                    return {false, "ERROR!!!"};
                }
                if (error & EntireUnitReader::DiscardedDataDueToEofError) {
                    qWarning("Discarded data while encoding mp3!");
                }

                int encodeBufferResult{};
                if (audioFormat.channelCount() == 1) {
                    encodeBufferResult = lame_encode_buffer(gfp.get(), samples.data(), nullptr,
                                       int(framesRead), mp3buffer.data(),
                                       int(mp3buffer.size()));
                } else if (audioFormat.channelCount() == 2) {
                    encodeBufferResult = lame_encode_buffer_interleaved(gfp.get(), samples.data(),
                                       int(framesRead), mp3buffer.data(),
                                       int(mp3buffer.size()));
                } else {
                    Q_ASSERT(false);
                }

                if (encodeBufferResult < 0) {
                    return {false, QString("Lame Error: %1").arg(encodeBufferResult)};
                }

                if (encodeBufferResult != writeFile.write(reinterpret_cast<char*>(mp3buffer.data()), encodeBufferResult)) {
                    return {false, QString("Write Error")};
                }

                if (framesRead < frameCount) {
                    break; // EOF
                }
            }

            const int flushBufferResult = lame_encode_flush(gfp.get(), mp3buffer.data(), int(mp3buffer.size()));
            if (flushBufferResult < 0) {
                return {false, QString("Lame Error: %1").arg(flushBufferResult)};
            }
            if (flushBufferResult != writeFile.write(reinterpret_cast<char*>(mp3buffer.data()), flushBufferResult)) {
                return {false, QString("Write Error")};
            }

            if (!writeFile.flush()) {
                return {false, QString("Write Error")};
            }
            writeFile.close();
            break;
        }
        case SupportedAudioFormat::Type::Int:
            break;
        }

        if (isInterruptionRequested()) {
            return {false, "Canceled"};
        }
    }

    return {true, ""};
}

} // namespace anonymous

BatchServiceImpl::BatchServiceImpl()
{
}

BatchServiceImpl::~BatchServiceImpl()
{
    if (!!workerThread_) {
        workerThread_->requestInterruption();
        workerThread_->wait();
    }
}

QVariant BatchServiceImpl::status() const
{
    return QVariant::fromValue(status_);
}

QString BatchServiceImpl::startRequestedExport(std::function<BatchServiceImplThread *()> creator)
{
    if (!!workerThread_) {
        return "Work is already running!!";
    }

    workerThread_ = decltype(workerThread_)(creator());

    QObject::connect(workerThread_.get(), &BatchServiceImplThread::finished,
                     this, &BatchServiceImpl::workerFinished,
                     Qt::QueuedConnection);
    QObject::connect(workerThread_.get(), &BatchServiceImplThread::statusTextChanged,
                     this, &BatchServiceImpl::workerStatusTextChanged,
                     Qt::QueuedConnection);

    status_.isWorking_ = true;
    status_.statusText_.clear();
    emit statusChanged(QVariant::fromValue(status_));
    emit androidStartService();

    workerThread_->start();

    return {};
}

QString BatchServiceImpl::requestExportJson(QUrl documentUrl)
{
    return startRequestedExport(
        [documentUrl]() { return new ExportJsonWorkerThread(documentUrl); });
}

QString BatchServiceImpl::requestExportPlayToFile(QUrl documentUrl)
{
    return startRequestedExport(
        [documentUrl]() { return new ExportPlayToFileWorkerThread(documentUrl); });
}

QString BatchServiceImpl::requestExportHtml(QUrl documentUrl)
{
    return startRequestedExport(
                [documentUrl]() { return new ExportHtmlWorkerThread(documentUrl); });
}

void BatchServiceImpl::requestCancelWorker()
{
    if (!!workerThread_) {
        workerThread_->requestInterruption();
    }
}

void BatchServiceImpl::applicationExiting()
{

}

void BatchServiceImpl::workerFinished()
{
    // XXX I *think* but am not positive it's okay to delete the thread now,
    //     since this is a queued connection.
    workerThread_.reset();

    status_.isWorking_ = false;
    emit statusChanged(QVariant::fromValue(status_));
    emit androidStopService();
}

void BatchServiceImpl::workerStatusTextChanged(const QString& fileName,
                                               const QString& statusText)
{
    status_.fileName_ = fileName;
    status_.statusText_ = statusText;
    emit statusChanged(QVariant::fromValue(status_));
}

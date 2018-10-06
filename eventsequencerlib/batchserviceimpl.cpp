#include "batchserviceimpl.h"

#include "audioformatholder.h"
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
#include "mp3encoder.h"

#include <QDir>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <cmath>

BatchServiceImplThread::BatchServiceImplThread(QUrl initialUrl)
{
    fileName_ = QFileInfo(initialUrl.toLocalFile()).completeBaseName();
}

void BatchServiceImplThread::run()
{
    // Some exporters don't report progress. Try not to have a blank status
    // when it's running.
    reportStatus("Exporting");

    auto result = process();
    if (result.success) {
        if (result.message.isEmpty()) {
            emit statusTextChanged(fileName_, QString("Success"), true);
        } else {
            emit statusTextChanged(fileName_, QString("Success: %1").arg(result.message), true);
        }
    } else {
        emit statusTextChanged(fileName_, QString("Failure: %1").arg(result.message), true);
    }
}

void BatchServiceImplThread::setFileNameFromPath(QString filePath)
{
    fileName_ = QFileInfo(filePath).fileName();
}

void BatchServiceImplThread::reportStatus(QString status)
{
    emit statusTextChanged(fileName_, status, false);
}

namespace {

class JsonExportSegment {
    int segmentStart;
    int length;
    QString text;
    bool hasConflict;
    QString fileAu;
    QString fileMp3;
    QString fileCreateTime;
public:
    void load(const QString& fileResourceDirectory, const QString& content,
              const channel::CollateChannel::Segment& segment);
    enum class FileType { Au, Mp3 };
    QJsonObject make(FileType ft);
};

void JsonExportSegment::load(const QString& fileResourceDirectory,
                             const QString& content,
                             const channel::CollateChannel::Segment& segment)
{
    this->segmentStart = segment.segmentStart;
    this->length = segment.segmentLength;
    this->text = content.mid(segment.segmentStart, segment.segmentLength);
    this->hasConflict =
        segment.segmentType == channel::CollateChannel::SegmentType::Conflict;

    Strip* s = segment.strip;
    if (s == nullptr) {
        return;
    }
    QUrl url = s->resourceUrl();
    if (url.isEmpty()) {
        return;
    }
    QString filePath;
    if (!ManagedResources(fileResourceDirectory)
             .urlConvertToFilePath(url, &filePath)) {
        return;
    }
    QFileInfo fi(filePath);
    if (fi.exists()) {
        this->fileAu = fi.fileName();
        this->fileMp3 = fi.completeBaseName() + ".mp3";

        // Let's make an attempt to read meta-data. Failures are
        // silently ignored because files may not have meta-data, may
        // be missing, etc.
        std::string createTime;
        if (ResourceMetaData::readFromFile(filePath, &createTime)) {
            this->fileCreateTime = QString::fromStdString(createTime);
        }
    }
}

QJsonObject JsonExportSegment::make(JsonExportSegment::FileType ft)
{
    QJsonObject obj;
    obj["startPosition"] = this->segmentStart;
    obj["length"] = this->length;
    obj["text"] = this->text;
    if (this->hasConflict) {
        obj["hasConflict"] = true;
    }
    if (ft == FileType::Au && !this->fileAu.isEmpty()) {
        obj["file"] = this->fileAu;
    } else if (ft == FileType::Mp3 && !this->fileMp3.isEmpty()) {
        obj["file"] = this->fileMp3;
    }
    if (!this->fileCreateTime.isEmpty()) {
        obj["fileCreateTime"] = this->fileCreateTime;
    }
    return obj;
}

class JsonExportBuilder {
    QJsonArray segments_;
    JsonExportSegment::FileType fileType_;
public:
    JsonExportBuilder(JsonExportSegment::FileType fileType)
        : fileType_(fileType)
    {
    }

    void addSegment(const QString& fileResourceDirectory,
                    const QString& content,
                    const channel::CollateChannel::Segment& segment)
    {
        JsonExportSegment jseg;
        jseg.load(fileResourceDirectory, content, segment);
        segments_.push_back(jseg.make(fileType_));
    }

    void addSegments(
        const QString& fileResourceDirectory, const QString& content,
        const std::vector<channel::CollateChannel::Segment>& segments)
    {
        for (auto& segment : segments) {
            addSegment(fileResourceDirectory, content, segment);
        }
    }

    QByteArray build()
    {
        QJsonDocument doc(segments_);
        return doc.toJson();
    }
};

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

    JsonExportBuilder exporter(JsonExportSegment::FileType::Au);
    exporter.addSegments(document.fileResourceDirectory(), content,
                         dfstructure.collateChannel->segments());

    QFile file(outputPath);
    if (!file.open(QFile::NewOnly)) {
        return {false, QString("Cannot open file: %1").arg(file.errorString())};
    }

    QByteArray toWrite = exporter.build();
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

class AutoFileDeletion {
    QFile& file_;
    bool committed_ = false;
public:
    AutoFileDeletion(QFile& file) : file_(file) {}
    ~AutoFileDeletion()
    {
        if (!committed_) {
            if (!file_.remove()) {
                qWarning("Unable to remove temp file");
            }
        }
    }
    void commit()
    {
        committed_ = true;
    }
};

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

    AutoFileDeletion fileDeleter(file);

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

        if (QThread::currentThread()->isInterruptionRequested()) {
            return {false, "Canceled"};
        }
    }

    if (!file.flush()) {
        return {false, QString("Cannot flush: %1").arg(file.errorString())};
    }

    file.close();
    fileDeleter.commit();

    return {true, ""};
}

class ExportHtmlWorkerThread : public BatchServiceImplThread {
    QUrl documentUrl_;
    bool merge_;
public:
    ExportHtmlWorkerThread(QUrl documentUrl, bool merge)
        : BatchServiceImplThread(documentUrl),
          documentUrl_(documentUrl),
          merge_(merge)
    {
    }

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

    QDir outputDir(outputPath);
    if (!merge_) {
        if (outputDir.exists()) {
            return {false, "Already exists!"};
        }
    }
    if (!outputDir.mkpath(".")) {
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
    // I think LAME expects the byte order of the host.
    audioFormat.setByteOrder(static_cast<QAudioFormat::Endian>(QSysInfo::ByteOrder));

    ManagedResources managedResources(document.fileResourceDirectory());

    struct WorkItem {
        QUrl sourceUrl;
        QString sourceFilePath;
        QString destFileName;
    };
    std::vector<WorkItem> workItems;

    auto allSegments = dfstructure.collateChannel->segments();
    for (auto& s : allSegments) {
        if (s.strip != nullptr) {
            if (!s.strip->resourceUrl().isEmpty()) {
                WorkItem wi;
                wi.sourceUrl = s.strip->resourceUrl();
                if (!managedResources.urlConvertToFilePath(
                        s.strip->resourceUrl(), &wi.sourceFilePath)) {
                    return {false, "Internal error"};
                }
                QFileInfo fi(wi.sourceFilePath);
                if (!fi.isFile()) {
                    return {false,
                            QString("Missing file: %1").arg(wi.sourceFilePath)};
                }
                wi.destFileName = fi.completeBaseName() + ".mp3";

                workItems.push_back(wi);
            }
        }
    }

    if (merge_) {
        std::set<QString> filesWeShouldHave;
        for (auto& wi : workItems) {
            filesWeShouldHave.insert(wi.destFileName);
        }

        std::set<QString> filesWeAlreadyHave;
        std::vector<QString> filesWeShouldDelete;
        for (auto& fi : outputDir.entryInfoList(QDir::Files)) {
            if (filesWeShouldHave.count(fi.fileName()) == 0) {
                // We shouldn't have this file.
                filesWeShouldDelete.push_back(fi.fileName());
            } else {
                // We should have this file.
                filesWeAlreadyHave.insert(fi.fileName());
            }
        }

        for (auto& s : filesWeShouldDelete) {
            QFile::remove(outputDir.filePath(s));
        }

        workItems.erase(std::remove_if(workItems.begin(), workItems.end(),
                                       [&](WorkItem& wi) {
                                           return filesWeAlreadyHave.count(
                                                      wi.destFileName) != 0;
                                       }),
                        workItems.end());
    }

    for (size_t i = 0; i < workItems.size(); ++i) {
        WorkItem& wi = workItems[i];

        reportStatus(QString("%1 of %2 files encoded").arg(i).arg(workItems.size()));

        QString partialOutputFilePath = QString("%1/%2.part").arg(outputPath).arg(wi.destFileName);
        QString outputFilePath = QString("%1/%2").arg(outputPath).arg(wi.destFileName);

        std::unique_ptr<SampleModifyingIODevice> resource;
        {
            EndianModifyingIODevice* emiod{};
            QString errorMessage;
            if (playable::PlayableBase::buildEmiod(
                    document.fileResourceDirectory(), wi.sourceUrl,
                    audioFormat, &emiod, &errorMessage)) {
                resource.reset(emiod);
            } else {
                return {false, QString("Error: %1: %2").arg(wi.sourceUrl.toString()).arg(errorMessage)};
            }
        }

        {
            const bool ok = resource->open(QIODevice::ReadOnly);
            Q_ASSERT(ok); // Can't happen, but just make sure.
        }

        QFile writeFile(partialOutputFilePath);
        if (!writeFile.open(QFile::WriteOnly)) {
            return {false, QString("Error: %1: %2").arg(outputFilePath).arg(writeFile.errorString())};
        }
        AutoFileDeletion fileDeleter(writeFile);

        Mp3Encoder mp3enc(resource.get(), &audioFormat, &writeFile);
        {
            auto result = mp3enc.initAndEncode();
            if (!result.success) {
                return {false, result.errorMessage};
            }
        }
        if (!writeFile.flush()) {
            return {false, QString("Write Error")};
        }
        writeFile.close();

        if (!QFile::rename(partialOutputFilePath, outputFilePath)) {
            return {false, "Cannot rename partial file"};
        }

        fileDeleter.commit();

        if (isInterruptionRequested()) {
            return {false, "Canceled"};
        }
    }

    reportStatus("Building HTML");
    {
        QFile htmlFile(outputDir.filePath("index.html"));
        if (!htmlFile.open(QIODevice::WriteOnly)) {
            return {false, "Cannot open html file for writing"};
        }
        AutoFileDeletion fileDeleter(htmlFile);

        {
            QString content = dfstructure.textChannel->content();
            JsonExportBuilder jsonExporter(JsonExportSegment::FileType::Mp3);
            jsonExporter.addSegments(document.fileResourceDirectory(), content,
                                     dfstructure.collateChannel->segments());
            auto result = jsonExporter.build();
            if (htmlFile.write(result) != result.size()) {
                return {false, "Write failed"};
            }
        }

        if (!htmlFile.flush()) {
            return {false, "Flush failed"};
        }

        htmlFile.close();
        fileDeleter.commit();
    }

    return {true, ""};
}

} // namespace anonymous

BatchServiceImpl::BatchServiceImpl()
{
    QObject::connect(this, &BatchServiceImpl::statusChanged,
                     this, &BatchServiceImplBase::parentStatusChanged);
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

QString BatchServiceImpl::requestExportHtml(QUrl documentUrl, bool merge)
{
    return startRequestedExport([documentUrl, merge]() {
        return new ExportHtmlWorkerThread(documentUrl, merge);
    });
}

void BatchServiceImpl::requestCancelWorker()
{
    if (!!workerThread_) {
        workerThread_->requestInterruption();
    }
}

void BatchServiceImpl::requestClearStatus()
{
    if (!!workerThread_) {
        qWarning("Refusing to clear status when worker is running");
        return;
    }

    status_.isWorking_ = false;
    status_.isResult_ = false;
    status_.fileName_.clear();
    status_.statusText_.clear();
    emit statusChanged(QVariant::fromValue(status_));
}

void BatchServiceImpl::applicationExiting()
{

}

void BatchServiceImpl::workerFinished()
{
    // XXX I *think* but am not positive it's okay to delete the thread now,
    //     since this is a queued connection.
    workerThread_.reset();

    emit androidStopService();
    status_.isWorking_ = false;
    status_.isResult_ = false;
    emit statusChanged(QVariant::fromValue(status_));
}

void BatchServiceImpl::workerStatusTextChanged(const QString& fileName,
                                               const QString& statusText,
                                               bool isResult)
{
    status_.isResult_ = isResult;
    status_.fileName_ = fileName;
    status_.statusText_ = statusText;
    emit statusChanged(QVariant::fromValue(status_));
}

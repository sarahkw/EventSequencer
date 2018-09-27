#include "batchserviceimpl.h"

#include "batchservicestatus.h"
#include "document.h"
#include "docfillstructure.h"
#include "managedresources.h"
#include "resourcemetadata.h"
#include "channel/textchannel.h"
#include "channel/collatechannel.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace {

class ExportJsonWorkerThread : public BatchServiceImplThread {
    QUrl documentUrl_;
public:
    ExportJsonWorkerThread(QUrl documentUrl) : documentUrl_(documentUrl) {}
protected:
    void run() override;
};

void ExportJsonWorkerThread::run()
{
    Document document;
    {
        auto result = document.load(documentUrl_);
        if (!result[0].toBool()) {
            emit statusTextChanged(QString("Cannot open: %1").arg(result[1].toString()));
            return;
        }
    }
    QString outputPath;
    {
        DocumentPathsResponse pathResponse;
        if (document.pathQuery(DocumentPaths::PathRequest::JSON_EXPORT, &pathResponse)) {
            outputPath = pathResponse.filePath;
        } else {
            emit statusTextChanged("Internal error");
            return;
        }
    }

    DocFillStructure dfstructure;
    if (!dfstructure.load(document)) {
        emit statusTextChanged("Internal error");
        return;
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
        emit statusTextChanged(QString("Cannot open file: %1").arg(file.errorString()));
        return;
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

    emit statusTextChanged(error);
    return;
}

class ExportPlayToFileWorkerThread : public BatchServiceImplThread {
protected:
    void run() override
    {
        for (int i = 10; i > 0; --i) {
            emit statusTextChanged(QString("Export Play to File: %1").arg(i));
            QThread::sleep(1);
        }
    }
};

class ExportHtmlWorkerThread : public BatchServiceImplThread {
protected:
    void run() override
    {
        for (int i = 30; i > 0; --i) {
            emit statusTextChanged(QString("Export HTML: %1").arg(i));
            QThread::sleep(1);
            if (isInterruptionRequested()) {
                return;
            }
        }
        emit statusTextChanged(QString("Export HTML: Success"));
    }
};

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
    /*
    if (workSimulator_.isActive()) {
        return "Work is already running!!";
    }

    qInfo() << "requestExportPlayToFile" << documentUrl;

    workLeft_ = 30;
    emit statusChanged(workLeft_);
    emit androidStartService();
    workSimulator_.start();

    updateStatus();
    */
    return {};
}

QString BatchServiceImpl::requestExportHtml(QUrl documentUrl)
{
    return startRequestedExport([]() { return new ExportHtmlWorkerThread; });
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

void BatchServiceImpl::workerStatusTextChanged(const QString &statusText)
{
    status_.statusText_ = statusText;
    emit statusChanged(QVariant::fromValue(status_));
}

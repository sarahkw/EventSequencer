#include "docfillbackfillstats.h"

#include "channel/docfillchannel.h"
#include "channel/collatechannel.h"
#include "qmlresourcemetadatagetter.h"
#include "describeduration.h"

#include <memory>

DocFillDatabase *DocFillBackfillStats::docfillDatabase() const
{
    return docfillDatabase_;
}

void DocFillBackfillStats::setDocfillDatabase(DocFillDatabase *docfillDatabase)
{
    if (docfillDatabase_ != docfillDatabase) {
        if (docfillDatabase_ != nullptr) {
            docfillDatabase_->disconnect(this);
        }
        docfillDatabase_ = docfillDatabase;
        if (docfillDatabase_ != nullptr) {
            QObject::connect(docfillDatabase_, &QObject::destroyed,
                             this, &DocFillBackfillStats::clearDocfillDatabase);
        }
        emit docfillDatabaseChanged();
        updateReport();
    }
}

void DocFillBackfillStats::clearDocfillDatabase()
{
    setDocfillDatabase(nullptr);
}

QString DocFillBackfillStats::filePath() const
{
    return filePath_;
}

void DocFillBackfillStats::setFilePath(const QString &filePath)
{
    if (filePath_ != filePath) {
        filePath_ = filePath;
        emit filePathChanged();
        updateReport();
    }
}

QString DocFillBackfillStats::report() const
{
    return report_;
}

void DocFillBackfillStats::updateReport()
{
    if (docfillDatabase_ == nullptr || filePath_.isEmpty()) {
        rawData_.clear();
        report_.clear();
        emit reportChanged();
        return;
    }

    {
        auto result = document_.loadFilePath(filePath_);
        if (!result[0].toBool()) {
            report_ = QString("Cannot load document: %1").arg(result[1].toString());
            emit reportChanged();
            return;
        }
    }

    QmlResourceMetaDataGetter metaGet;
    metaGet.setFileResourceDirectory(document_.fileResourceDirectory());

#define DOCUMENT_INVALID_IF(cond) do { if (cond) { report_ = "Invalid document"; emit reportChanged(); return; } } while (false)

    auto* programChannel = qobject_cast<channel::DocFillChannel*>(document_.defaultProgramChannel());
    DOCUMENT_INVALID_IF(programChannel == nullptr);

    std::unique_ptr<WaitFor> collateChannelWaitFor(document_.waitForChannelIndex(programChannel->resourceChannel()));
    auto* collateChannel = qobject_cast<channel::CollateChannel*>(collateChannelWaitFor->result());
    DOCUMENT_INVALID_IF(collateChannel == nullptr);

    std::unique_ptr<WaitFor> stripSourceWaitFor(document_.waitForChannelIndex(collateChannel->channel()));
    auto* stripSource = qobject_cast<channel::ChannelBase*>(stripSourceWaitFor->result());
    DOCUMENT_INVALID_IF(stripSource == nullptr);
#undef DOCUMENT_INVALID_IF

    size_t numSuccess = 0;
    size_t numFailure = 0;

    for (Strip* s : stripSource->multiChannelStrips()) {
        auto& cache = metaGet.getRaw(s->resourceUrl());
        if (cache.success_ && cache.createTime.isValid()) {
            ++numSuccess;

           auto localDate = cache.createTime.toLocalTime().date();
           int yyyymmdd = localDate.year() * 10000 + localDate.month() * 100 + localDate.day();
           rawData_[yyyymmdd] += cache.durationInMicroSeconds;
        } else {
            ++numFailure;
        }
    }

    report_ = QString("Success = %1\nFailure = %2\n\n").arg(numSuccess).arg(numFailure);

    for (auto& pair : rawData_) {
        report_.append(QString("%1 %2\n").arg(pair.first).arg(DescribeDuration::describeDuration(pair.second / 1000 / 1000)));
    }

    emit reportChanged();
}

DocFillBackfillStats::DocFillBackfillStats(QObject *parent) : QObject(parent)
{

}

QString DocFillBackfillStats::actuallyBackfill()
{
    if (rawData_.empty()) {
        return "Nothing to backfill";
    }
    if (docfillDatabase_ == nullptr) {
        return "Internal error";
    }
    return docfillDatabase_->statsMerge(rawData_);
}

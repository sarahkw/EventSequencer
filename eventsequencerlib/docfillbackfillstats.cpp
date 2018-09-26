#include "docfillbackfillstats.h"

#include "channel/channelbase.h"
#include "docfillstructure.h"
#include "qmlresourcemetadatagetter.h"
#include "describeduration.h"

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

    DocFillStructure dfstructure;
    if (!dfstructure.load(document_)) {
        report_ = "Invalid document";
        emit reportChanged();
        return;
    }

    size_t numSuccess = 0;
    size_t numFailure = 0;

    for (Strip* s : dfstructure.stripSourceChannel->multiChannelStrips()) {
        auto& cache = metaGet.getRaw(s->resourceUrl());
        if (cache.success_ && cache.createTime.isValid()) {
            ++numSuccess;
            rawData_[cache.createTimeAsLocalYYYYMMDD()] += cache.durationInMicroSeconds;
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

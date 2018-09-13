#include "qmlresourcemetadatagetter.h"

#include "resourcemetadata.h"
#include "managedresources.h"
#include "describeduration.h"

#include <QDebug>

const QmlResourceMetaDataGetter::CacheObject &QmlResourceMetaDataGetter::getRaw(QUrl resourceUrl)
{
    {
        auto iter = cache_.find(resourceUrl);
        if (iter != cache_.end()) {
            return iter->second;
        }
    }

    CacheObject mine;
    QString filePath;
    if (ManagedResources(fileResourceDirectory_).urlConvertToFilePath(resourceUrl, &filePath)) {
        AuFileHeader::FileInformation afh_fi;
        qint64 createTimeInSeconds{};
        if (ResourceMetaData::readFromFile(filePath, nullptr, &createTimeInSeconds, &afh_fi)) {
            if (createTimeInSeconds > 0) {
                // XXX This kinda sucks. We can have duration without having
                //     create time. It's just that for the current business logic
                //     it's useless to have duration if we don't also have create
                //     time. Maybe one day we'll need to make them independent.
                auto dt = QDateTime::fromSecsSinceEpoch(createTimeInSeconds);
                if (dt.isValid()) {
                    mine.createTime = dt;
                    mine.durationInMicroSeconds = afh_fi.durationInMicroSeconds;
                    mine.success_ = true;
                }
            }
        }
    }
    if (!mine.success_) {
        qWarning() << "Unable to get metadata for" << resourceUrl;
    }

    cache_[resourceUrl] = mine;
    return cache_[resourceUrl];
}

QmlResourceMetaDataGetter::QmlResourceMetaDataGetter(QObject *parent) : QObject(parent)
{

}

QString QmlResourceMetaDataGetter::fileResourceDirectory() const
{
    return fileResourceDirectory_;
}

void QmlResourceMetaDataGetter::setFileResourceDirectory(const QString &fileResourceDirectory)
{
    if (fileResourceDirectory_ != fileResourceDirectory) {
        fileResourceDirectory_ = fileResourceDirectory;
        emit fileResourceDirectoryChanged();
    }
}

QDateTime QmlResourceMetaDataGetter::getCreateTime(QUrl resourceUrl)
{
    return getRaw(resourceUrl).createTime;
}

int QmlResourceMetaDataGetter::getCreateTimeLocalYYYYMMDD(QUrl resourceUrl)
{
    auto& cache = getRaw(resourceUrl);
    if (cache.success_) {
        return cache.createTimeAsLocalYYYYMMDD();
    } else {
        return -1;
    }
}

qint64 QmlResourceMetaDataGetter::getDurationInMicroSeconds(QUrl resourceUrl)
{
    return getRaw(resourceUrl).durationInMicroSeconds;
}

QString QmlResourceMetaDataGetter::getDurationDescription(QUrl resourceUrl)
{
    return DescribeDuration::describeDuration(
        getDurationInMicroSeconds(resourceUrl) / 1000 / 1000);
}

int QmlResourceMetaDataGetter::CacheObject::createTimeAsLocalYYYYMMDD() const
{
    auto localDate = this->createTime.toLocalTime().date();
    return localDate.year() * 10000 + localDate.month() * 100 + localDate.day();
}

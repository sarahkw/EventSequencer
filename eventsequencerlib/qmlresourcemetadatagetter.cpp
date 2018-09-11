#include "qmlresourcemetadatagetter.h"

#include "resourcemetadata.h"
#include "managedresources.h"

#include <QDebug>

const QmlResourceMetaDataGetter::CacheObject &QmlResourceMetaDataGetter::get(QUrl resourceUrl)
{
    {
        auto iter = cache_.find(resourceUrl);
        if (iter != cache_.end()) {
            return iter->second;
        }
    }

    CacheObject mine;
    QString filePath;
    bool good = false;
    if (ManagedResources(fileResourceDirectory_).urlConvertToFilePath(resourceUrl, &filePath)) {
        AuFileHeader::FileInformation afh_fi;
        qint64 createTimeInSeconds{};
        if (ResourceMetaData::readFromFile(filePath, nullptr, &createTimeInSeconds, &afh_fi)) {
            mine.createTime = QDateTime::fromSecsSinceEpoch(createTimeInSeconds);
            mine.durationInMicroSeconds = afh_fi.durationInMicroSeconds;
            good = true;
        }
    }
    if (!good) {
        qWarning() << "Unable to get metadata for" << resourceUrl << "- using blank values";
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
    return get(resourceUrl).createTime;
}

qint64 QmlResourceMetaDataGetter::getDurationInMicroSeconds(QUrl resourceUrl)
{
    return get(resourceUrl).durationInMicroSeconds;
}

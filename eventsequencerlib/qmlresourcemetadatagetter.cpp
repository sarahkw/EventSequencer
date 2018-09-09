#include "qmlresourcemetadatagetter.h"

#include "resourcemetadata.h"
#include "managedresources.h"

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

QDateTime QmlResourceMetaDataGetter::get(QUrl resourceUrl)
{
    {
        auto iter = cache_.find(resourceUrl);
        if (iter != cache_.end()) {
            return iter->second;
        }
    }

    QDateTime mine;
    QString filePath;
    if (ManagedResources(fileResourceDirectory_).urlConvertToFilePath(resourceUrl, &filePath)) {
        qint64 createTimeInSeconds{};
        if (ResourceMetaData::readFromFile(filePath, nullptr, &createTimeInSeconds)) {
            mine = QDateTime::fromSecsSinceEpoch(createTimeInSeconds);
        }
    }

    cache_[resourceUrl] = mine;
    return mine;
}

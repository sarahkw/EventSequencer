#include "managedresources.h"

#include <QFile>
#include <QDir>
#include <QUuid>
#include <QDebug>

void ManagedResources::mkpathManagedDirectory()
{
    if (fileResourceDirectory().isEmpty()) {
        return;
    }
    QDir dir(fileResourceDirectory());
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

QString ManagedResources::fileResourceDirectory() const
{
    return fileResourceDirectory_;
}

void ManagedResources::setFileResourceDirectory(const QString &fileResourceDirectory)
{
    if (fileResourceDirectory_ != fileResourceDirectory) {
        fileResourceDirectory_ = fileResourceDirectory;
        emit fileResourceDirectoryChanged();
    }
}

QString ManagedResources::generateResourceBaseName()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QUrl ManagedResources::generateResourceUrl(QString suffix)
{
    return urlForFileName(generateResourceBaseName() + suffix);
}

QUrl ManagedResources::urlForFileName(QString fileName)
{
    mkpathManagedDirectory();
    return "evseq://managed/" + fileName;
}

QVariantList ManagedResources::renameUrlToFileName(QUrl url, QString newFileName)
{
    QString fileName;
    if (!urlConvertToFilePath(url, &fileName)) return {};

    QFile f(fileName);
    if (f.rename(QFileInfo(fileName).dir().path() + "/" + newFileName)) {
        if (urlIsManaged(url)) {
            return {true, urlForFileName(newFileName)};
        } else {
            return {true, QUrl::fromLocalFile(f.fileName())};
        }
    } else {
        return {false, f.errorString()};
    }
}

QVariantList ManagedResources::renameUrlToGeneratedFileName(QUrl url, QString suffix)
{
    return renameUrlToFileName(url, generateResourceBaseName() + suffix);
}

bool ManagedResources::deleteUrl(QUrl url)
{
    QString fileName;
    if (!urlConvertToFilePath(url, &fileName)) return false;

    return QFile::remove(fileName);
}

bool ManagedResources::existsUrl(QUrl url)
{
    QString fileName;
    if (!urlConvertToFilePath(url, &fileName)) return false;

    return QFile::exists(fileName);
}

bool ManagedResources::urlIsManaged(QUrl url)
{
    return url.scheme() == "evseq" && url.host() == "managed";
}

bool ManagedResources::urlConvertToFilePath(QUrl url, QString *filePath)
{
    Q_ASSERT(filePath != nullptr);
    if (urlIsManaged(url)) {
        if (fileResourceDirectory().isEmpty()) {
            return false;
        }
        *filePath = fileResourceDirectory() + url.path();
    } else {
        *filePath = url.toLocalFile();
    }
    return true;
}

bool ManagedResources::urlFile(QUrl url, QFile *file)
{
    QString filePath;
    if (!urlConvertToFilePath(url, &filePath)) return false;
    file->setFileName(filePath);
    return true;
}


ManagedResources::ManagedResources(QObject *parent) : QObject(parent)
{

}

ManagedResources::ManagedResources(QString fileResourceDirectory, QObject *parent)
    : fileResourceDirectory_(fileResourceDirectory)
{

}

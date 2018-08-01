#include "managedresources.h"

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

QString ManagedResources::generateResourceName()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QString ManagedResources::withSpecifiedName(QString name, QString suffix)
{
    mkpathManagedDirectory();
    return "evseq://managed/" + name + suffix;
}

bool ManagedResources::deleteUrl(QUrl url)
{
    QString fileName;
    if (!convertToFileName(url, &fileName)) return false;

    return QFile::remove(fileName);
}

bool ManagedResources::convertToFileName(QUrl url, QString *fileName)
{
    Q_ASSERT(fileName != nullptr);
    if (url.scheme() == "evseq" && url.host() == "managed") {
        if (fileResourceDirectory().isEmpty()) {
            return false;
        }
        *fileName = fileResourceDirectory() + url.path();
    } else {
        *fileName = url.toLocalFile();
    }
    return true;
}

ManagedResources::ManagedResources(QObject *parent) : QObject(parent)
{

}

ManagedResources::ManagedResources(QString fileResourceDirectory, QObject *parent)
    : fileResourceDirectory_(fileResourceDirectory)
{

}

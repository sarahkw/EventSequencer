#include "managedresources.h"

#include <QDir>
#include <QUuid>

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
    if (fileResourceDirectory_.isEmpty()) {
        return "";
    }
    QDir dir(fileResourceDirectory_);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dir.absoluteFilePath(name + suffix);
}

bool ManagedResources::deleteFile(QString filePath)
{
    return QFile::remove(filePath);
}

ManagedResources::ManagedResources(QObject *parent) : QObject(parent)
{

}

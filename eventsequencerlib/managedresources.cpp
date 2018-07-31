#include "managedresources.h"

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

ManagedResources::ManagedResources(QObject *parent) : QObject(parent)
{

}

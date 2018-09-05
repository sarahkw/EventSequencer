#include "qmlfilereader.h"

#include <QFile>
#include <QDebug>

QmlFileReader::QmlFileReader(QObject *parent) : QObject(parent)
{

}

QString QmlFileReader::readFile(QString filePath) const
{
    QFile file(filePath);
    if (file.open(QFile::ReadOnly)) {
        return QString(file.readAll());
    }
    qWarning() << "Unable to read" << filePath;
    return "";
}

#ifndef QMLFILEREADER_H
#define QMLFILEREADER_H

#include <QObject>

class QmlFileReader : public QObject
{
    Q_OBJECT
public:
    explicit QmlFileReader(QObject *parent = nullptr);

    Q_INVOKABLE QString readFile(QString filePath) const;

signals:

public slots:
};

#endif // QMLFILEREADER_H

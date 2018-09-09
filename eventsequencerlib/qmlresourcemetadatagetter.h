#ifndef QMLRESOURCEMETADATAGETTER_H
#define QMLRESOURCEMETADATAGETTER_H

#include <QObject>
#include <QDateTime>
#include <QUrl>

#include <map>

class QmlResourceMetaDataGetter : public QObject
{
    Q_OBJECT

    std::map<QUrl, QDateTime> cache_;

    QString fileResourceDirectory_;
    Q_PROPERTY(QString fileResourceDirectory READ fileResourceDirectory WRITE setFileResourceDirectory NOTIFY fileResourceDirectoryChanged)

public:
    explicit QmlResourceMetaDataGetter(QObject *parent = nullptr);

    QString fileResourceDirectory() const;
    void setFileResourceDirectory(const QString &fileResourceDirectory);

    Q_INVOKABLE QDateTime get(QUrl resourceUrl);

signals:

    void fileResourceDirectoryChanged();


public slots:
};

#endif // QMLRESOURCEMETADATAGETTER_H

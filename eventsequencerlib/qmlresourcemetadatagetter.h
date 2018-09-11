#ifndef QMLRESOURCEMETADATAGETTER_H
#define QMLRESOURCEMETADATAGETTER_H

#include <QObject>
#include <QDateTime>
#include <QUrl>

#include <map>

class QmlResourceMetaDataGetter : public QObject
{
    Q_OBJECT

    struct CacheObject {
        QDateTime createTime;
        qint64 durationInMicroSeconds = 0;
    };

    // TODO I think we should cap the amount of memory we use, eventually. For now
    //      I don't think it should even hit a MB.
    std::map<QUrl, CacheObject> cache_;

    QString fileResourceDirectory_;
    Q_PROPERTY(QString fileResourceDirectory READ fileResourceDirectory WRITE setFileResourceDirectory NOTIFY fileResourceDirectoryChanged)

    const CacheObject& get(QUrl resourceUrl);

public:
    explicit QmlResourceMetaDataGetter(QObject *parent = nullptr);

    QString fileResourceDirectory() const;
    void setFileResourceDirectory(const QString &fileResourceDirectory);

    Q_INVOKABLE QDateTime getCreateTime(QUrl resourceUrl);
    Q_INVOKABLE qint64 getDurationInMicroSeconds(QUrl resourceUrl);

signals:

    void fileResourceDirectoryChanged();


public slots:
};

#endif // QMLRESOURCEMETADATAGETTER_H

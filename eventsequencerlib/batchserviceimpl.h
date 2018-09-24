#ifndef BATCHSERVICEIMPL_H
#define BATCHSERVICEIMPL_H

#include <QObject>
#include <QTimer>
#include <QVariant>

class BatchServiceImpl : public QObject
{
    Q_OBJECT

    QTimer workSimulator_;
    int workLeft_ = 0;

    QVariant status_;
    Q_PROPERTY(QVariant status READ status NOTIFY statusChanged)

public:

    BatchServiceImpl();

    QVariant status() const;
private:
    void setStatus(const QVariant &status);
public:

signals:

    void statusChanged(const QVariant& status);

    void androidStartService();
    void androidStopService();

public slots:

    void requestStartWork();

    void applicationExiting();

};

#endif // BATCHSERVICEIMPL_H

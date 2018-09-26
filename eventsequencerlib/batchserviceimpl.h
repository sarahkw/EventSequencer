#ifndef BATCHSERVICEIMPL_H
#define BATCHSERVICEIMPL_H

#include "batchservicestatus.h"

#include <QObject>
#include <QTimer>
#include <QVariant>
#include <QUrl>

class BatchServiceImpl : public QObject
{
    Q_OBJECT

    QTimer workSimulator_;
    int workLeft_ = 0;

    BatchServiceStatus status_;
    Q_PROPERTY(QVariant status READ status NOTIFY statusChanged)

public:

    BatchServiceImpl();

    QVariant status() const;
private:
    void updateStatus();
public:

    Q_INVOKABLE QString requestExportHtml(QUrl documentUrl);

signals:

    void statusChanged(const QVariant& status);

    void androidStartService();
    void androidStopService();

public slots:

    void applicationExiting();

};

#endif // BATCHSERVICEIMPL_H

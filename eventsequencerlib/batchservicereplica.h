#ifndef BATCHSERVICEREPLICA_H
#define BATCHSERVICEREPLICA_H

#include <QtGlobal>
#ifdef Q_OS_ANDROID

#include <androidlib/backendipc.h>

#include <QObject>
#include <QVariant>

class BatchServiceReplica : public QObject
{
    Q_OBJECT

    QVariant status_;
    Q_PROPERTY(QVariant status READ status NOTIFY statusChanged)

    // This must come after all members updated from remote, to
    // prevent possibly writing to a destructed member.
    androidlib::BackendIpc ipc_;

public:
    explicit BatchServiceReplica(QObject *parent = nullptr);

    QVariant status() const;

    Q_INVOKABLE QString requestExportHtml(QUrl documentUrl);

signals:

    void statusChanged(const QVariant& status);

public slots:


};

#endif // Q_OS_ANDROID

#endif // BATCHSERVICEREPLICA_H

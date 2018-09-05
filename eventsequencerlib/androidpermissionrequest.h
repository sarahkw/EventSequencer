#ifndef ANDROIDPERMISSIONREQUEST_H
#define ANDROIDPERMISSIONREQUEST_H

#include <QObject>

class AndroidPermissionRequest : public QObject
{
    Q_OBJECT
public:
    explicit AndroidPermissionRequest(QObject *parent = nullptr);

    Q_INVOKABLE void requestStorage() const;

    Q_INVOKABLE bool requestRecordAudio() const;

    //Q_INVOKABLE void requestPermissions() const;

signals:

public slots:
};

#endif // ANDROIDPERMISSIONREQUEST_H

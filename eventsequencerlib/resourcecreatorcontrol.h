#ifndef RESOURCECREATORCONTROL_H
#define RESOURCECREATORCONTROL_H

#include <QObject>

class ResourceCreatorControl : public QObject
{
    Q_OBJECT
public:
    explicit ResourceCreatorControl(QObject *parent = nullptr);

signals:

public slots:
};

#endif // RESOURCECREATORCONTROL_H
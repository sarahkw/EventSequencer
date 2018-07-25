#ifndef MANAGEDRESOURCES_H
#define MANAGEDRESOURCES_H

#include <QObject>

class ManagedResources : public QObject
{
    Q_OBJECT
public:
    explicit ManagedResources(QObject *parent = nullptr);

signals:

public slots:
};

#endif // MANAGEDRESOURCES_H
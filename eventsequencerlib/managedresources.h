#ifndef MANAGEDRESOURCES_H
#define MANAGEDRESOURCES_H

#include <QObject>

class ManagedResources : public QObject
{
    Q_OBJECT

    QString fileResourceDirectory_;
    Q_PROPERTY(QString fileResourceDirectory READ fileResourceDirectory WRITE setFileResourceDirectory NOTIFY fileResourceDirectoryChanged)

public:
    explicit ManagedResources(QObject *parent = nullptr);

    QString fileResourceDirectory() const;
    void setFileResourceDirectory(const QString &fileResourceDirectory);

signals:

    void fileResourceDirectoryChanged();

public slots:
};

#endif // MANAGEDRESOURCES_H

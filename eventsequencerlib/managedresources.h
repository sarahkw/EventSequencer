#ifndef MANAGEDRESOURCES_H
#define MANAGEDRESOURCES_H

#include <QObject>
#include <QUrl>

class ManagedResources : public QObject
{
    Q_OBJECT

    QString fileResourceDirectory_;
    Q_PROPERTY(QString fileResourceDirectory READ fileResourceDirectory WRITE setFileResourceDirectory NOTIFY fileResourceDirectoryChanged)

    void mkpathManagedDirectory();

public:
    explicit ManagedResources(QObject *parent = nullptr);

    QString fileResourceDirectory() const;
    void setFileResourceDirectory(const QString &fileResourceDirectory);

    Q_INVOKABLE QString generateResourceName();
    Q_INVOKABLE QString withSpecifiedName(QString name, QString suffix);

    Q_INVOKABLE bool deleteUrl(QUrl url);

signals:

    void fileResourceDirectoryChanged();

public slots:
};

#endif // MANAGEDRESOURCES_H

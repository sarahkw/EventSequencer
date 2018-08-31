#ifndef MANAGEDRESOURCES_H
#define MANAGEDRESOURCES_H

#include <QObject>
#include <QUrl>
#include <QVariantList>

class QFile;

class ManagedResources : public QObject
{
    Q_OBJECT

    QString fileResourceDirectory_;
    Q_PROPERTY(QString fileResourceDirectory READ fileResourceDirectory WRITE setFileResourceDirectory NOTIFY fileResourceDirectoryChanged)

    void mkpathManagedDirectory();

public:
    explicit ManagedResources(QObject *parent = nullptr);
    ManagedResources(QString fileResourceDirectory, QObject *parent = nullptr);

    QString fileResourceDirectory() const;
    void setFileResourceDirectory(const QString &fileResourceDirectory);

    Q_INVOKABLE QString generateResourceBaseName();
    Q_INVOKABLE QUrl generateResourceUrl(QString suffix);

    Q_INVOKABLE QUrl urlForFileName(QString fileName);

    Q_INVOKABLE QVariantList renameUrlToFileName(QUrl url, QString newFileName);
    Q_INVOKABLE QVariantList renameUrlToGeneratedFileName(QUrl url, QString suffix);

    Q_INVOKABLE QVariantList deleteUrl(QUrl url);
    Q_INVOKABLE bool existsUrl(QUrl url);

    static bool urlIsManaged(QUrl url);
    bool urlConvertToFilePath(QUrl url, QString* filePath);

    bool urlFile(QUrl url, QFile* file);

signals:

    void fileResourceDirectoryChanged();

public slots:
};

#endif // MANAGEDRESOURCES_H

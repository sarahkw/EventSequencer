#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QObject>
#include <QVariant>

class DocumentManager;

class DocumentManagerItem
{
    Q_GADGET
    friend class DocumentManager;
    QString displayName_;
    QString filePath_;
    Q_PROPERTY(QString displayName READ displayName CONSTANT)
    Q_PROPERTY(QString filePath READ filePath CONSTANT)
public:
    QString displayName() const;
    QString filePath() const;
};
Q_DECLARE_METATYPE(DocumentManagerItem)

class DocumentManager : public QObject
{
    Q_OBJECT

    QString documentsPath_;
    Q_PROPERTY(QString documentsPath READ documentsPath WRITE setDocumentsPath NOTIFY documentsPathChanged)

    QVariantList items_;
    Q_PROPERTY(QVariantList items READ items NOTIFY itemsChanged)

    void updateItems();

public:
    explicit DocumentManager(QObject *parent = nullptr);

    QString documentsPath() const;
    void setDocumentsPath(const QString &documentsPath);

    QVariantList items() const;

signals:

    void documentsPathChanged();
    void itemsChanged();

public slots:
};

#endif // DOCUMENTMANAGER_H

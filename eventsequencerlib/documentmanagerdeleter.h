#ifndef DOCUMENTMANAGERDELETER_H
#define DOCUMENTMANAGERDELETER_H

#include <QObject>
#include <QVariantList>

class DocumentManagerDeleter : public QObject
{
    Q_OBJECT

    QString filePath_;
    QVariantList queuedForDeletion_;
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(QVariantList queuedForDeletion READ queuedForDeletion NOTIFY queuedForDeletionChanged)

    void updateQueuedForDeletion();

public:
    explicit DocumentManagerDeleter(QObject *parent = nullptr);

    QString filePath() const;
    void setFilePath(const QString &filePath);

    QVariantList queuedForDeletion() const;

    Q_INVOKABLE QString actuallyDelete();

signals:

    void filePathChanged();
    void queuedForDeletionChanged();

public slots:
};

#endif // DOCUMENTMANAGERDELETER_H

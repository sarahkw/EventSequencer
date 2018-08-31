#ifndef MANAGEDRESOURCEREPORT_H
#define MANAGEDRESOURCEREPORT_H

#include <QObject>
#include <QVariantList>
#include <QDateTime>

class Document;

class ManagedResourceReportUnusedFile {
    Q_GADGET

    QString path_;
    qint64 size_;
    QDateTime metaDataCreateDate_;
};

class ManagedResourceReport : public QObject
{
    Q_OBJECT

    bool hasQuickData_ = false;
    bool hasData_ = false;
    QDateTime generatedAt_;

    int totalFileCount_ = 0;
    qint64 totalSizeInBytes_ = 0;
    QVariantList stripsMissingResource_;
    QVariantList unusedFiles_;

public:
    explicit ManagedResourceReport(QObject *parent = nullptr);

    Q_INVOKABLE void generateReport(Document* document);

    Q_INVOKABLE void clearReport()
    {
    }

    // Returns a report of file deletion success/failure
    Q_INVOKABLE QString deleteFiles(QStringList files)
    {
    }

signals:

public slots:
};

#endif // MANAGEDRESOURCEREPORT_H

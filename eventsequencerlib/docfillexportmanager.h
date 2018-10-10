#ifndef DOCFILLEXPORTMANAGER_H
#define DOCFILLEXPORTMANAGER_H

#include "batchservicefactory.h"
#include "document.h"

#include <QObject>

class Document;

class DocFillExportManager : public QObject
{
    Q_OBJECT

    BatchServiceFactory::Type batchService_;
    Q_PROPERTY(QVariant batchServiceStatus READ batchServiceStatus NOTIFY batchServiceStatusChanged)

    Document* document_ = nullptr;
    Q_PROPERTY(Document* document READ document WRITE setDocument NOTIFY documentChanged)

    QString defaultExportJsonOutputPath_;
    QString defaultPlayToFileOutputPath_;
    QString defaultExportHtmlOutputPath_;
    Q_PROPERTY(QString defaultExportJsonOutputPath READ defaultExportJsonOutputPath NOTIFY defaultOutputPathsChanged)
    Q_PROPERTY(QString defaultPlayToFileOutputPath READ defaultPlayToFileOutputPath NOTIFY defaultOutputPathsChanged)
    Q_PROPERTY(QString defaultExportHtmlOutputPath READ defaultExportHtmlOutputPath NOTIFY defaultOutputPathsChanged)

    bool defaultExportJsonOutputPathExists_ = false;
    bool defaultPlayToFileOutputPathExists_ = false;
    bool defaultExportHtmlOutputPathExists_ = false;
    Q_PROPERTY(bool defaultExportJsonOutputPathExists READ defaultExportJsonOutputPathExists NOTIFY defaultOutputPathsChanged)
    Q_PROPERTY(bool defaultPlayToFileOutputPathExists READ defaultPlayToFileOutputPathExists NOTIFY defaultOutputPathsChanged)
    Q_PROPERTY(bool defaultExportHtmlOutputPathExists READ defaultExportHtmlOutputPathExists NOTIFY defaultOutputPathsChanged)

    void updateDefaultOutputPaths();

public:
    explicit DocFillExportManager(QObject *parent = nullptr);

    QVariant batchServiceStatus();

    Document *document() const;
    void setDocument(Document *document);
    void clearDocument();

    QString defaultExportJsonOutputPath() const;
    QString defaultPlayToFileOutputPath() const;
    QString defaultExportHtmlOutputPath() const;

    // SERVICEREQUESTS VERSION 04
    Q_INVOKABLE QString requestExportJson(Document* document);
    Q_INVOKABLE QString requestExportPlayToFile(Document* document);
    Q_INVOKABLE QString requestExportHtml(Document* document, bool merge, int mp3Quality);
    Q_INVOKABLE void requestCancelWorker();
    Q_INVOKABLE void requestClearStatus();

    // TODO Delete -- temporary as an in-between while we migrate.
    Q_INVOKABLE void tempUpdateDefaultOutputPaths();

    bool defaultExportJsonOutputPathExists() const;
    bool defaultPlayToFileOutputPathExists() const;
    bool defaultExportHtmlOutputPathExists() const;

    Q_INVOKABLE bool deleteDefaultExportJsonOutputPath();
    Q_INVOKABLE bool deleteDefaultPlayToFileOutputPath();
    Q_INVOKABLE bool deleteDefaultExportHtmlOutputPath();

    Q_INVOKABLE QVariantList getExportHtmlIndexUrl() const;

signals:

    void batchServiceStatusChanged();

    void documentChanged();
    void defaultOutputPathsChanged();

public slots:
};

#endif // DOCFILLEXPORTMANAGER_H

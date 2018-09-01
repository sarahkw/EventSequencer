#ifndef DOCFILLEXPORTMANAGER_H
#define DOCFILLEXPORTMANAGER_H

#include <QObject>

class Document;

class DocFillExportManager : public QObject
{
    Q_OBJECT

    Document* document_ = nullptr;
    // Using QObject* just so that Document* can stay an incomplete type.
    Q_PROPERTY(QObject* document READ document WRITE setDocument NOTIFY documentChanged)

    QString defaultExportJsonOutputPath_;
    QString defaultPlayToFileOutputPath_;
    Q_PROPERTY(QString defaultExportJsonOutputPath READ defaultExportJsonOutputPath NOTIFY defaultOutputPathsChanged)
    Q_PROPERTY(QString defaultPlayToFileOutputPath READ defaultPlayToFileOutputPath NOTIFY defaultOutputPathsChanged)

    void updateDefaultOutputPaths();

public:
    explicit DocFillExportManager(QObject *parent = nullptr);

    QObject *document() const;
    void setDocument(QObject *document);
    void clearDocument();

    QString defaultExportJsonOutputPath() const;

    QString defaultPlayToFileOutputPath() const;

signals:

    void documentChanged();
    void defaultOutputPathsChanged();

public slots:
};

#endif // DOCFILLEXPORTMANAGER_H

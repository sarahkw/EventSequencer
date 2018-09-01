#ifndef MANAGEDRESOURCEREPORT_H
#define MANAGEDRESOURCEREPORT_H

#include <QObject>
#include <QVariantList>
#include <QDateTime>

class Document;

class ManagedResourceReport : public QObject
{
    Q_OBJECT

    // Ignored file names are excluded from being "unused"
    QVariantList ignoredFileNames_;
    Q_PROPERTY(QVariantList ignoredFileNames READ ignoredFileNames WRITE setIgnoredFileNames NOTIFY ignoredFileNamesChanged)

    bool hasData_ = false;

    QVariantList stripsMissingResource_;
    QVariantList unusedFiles_;
    QString unusedFilesSize_;
    QString usedFilesSize_;

    Q_PROPERTY(bool hasData READ hasData NOTIFY reportChanged)
    Q_PROPERTY(QVariantList stripsMissingResource READ stripsMissingResource NOTIFY reportChanged)
    Q_PROPERTY(QVariantList unusedFiles READ unusedFiles NOTIFY reportChanged)
    Q_PROPERTY(QString unusedFilesSize READ unusedFilesSize NOTIFY reportChanged)
    Q_PROPERTY(QString usedFilesSize READ usedFilesSize NOTIFY reportChanged)

public:
    explicit ManagedResourceReport(QObject *parent = nullptr);

    Q_INVOKABLE void generateReport(Document* document);

    Q_INVOKABLE void clearReport();

    // Returns a report of file deletion success/failure
    Q_INVOKABLE QString deleteFiles(QStringList files)
    {
    }

    QVariantList stripsMissingResource() const;

    bool hasData() const;

    QVariantList unusedFiles() const;
    QString unusedFilesSize() const;
    QString usedFilesSize() const;

    QVariantList ignoredFileNames() const;
    void setIgnoredFileNames(const QVariantList &ignoredFileNames);

signals:

    void ignoredFileNamesChanged();

    void reportChanged();

public slots:
};

#endif // MANAGEDRESOURCEREPORT_H

#ifndef DOCFILLSETTINGS_H
#define DOCFILLSETTINGS_H

#include <QObject>
#include <QSettings>

class DocFillSettings : public QObject
{
    Q_OBJECT

    QSettings settings_;

    QString defaultDocumentsPath_;
    QString documentsPath_;
    Q_PROPERTY(QString defaultDocumentsPath READ defaultDocumentsPath CONSTANT)
    Q_PROPERTY(QString documentsPath READ documentsPath WRITE setDocumentsPath NOTIFY documentsPathChanged)
    Q_PROPERTY(bool documentsPathIsDefault READ documentsPathIsDefault NOTIFY documentsPathChanged)

public:
    explicit DocFillSettings(QObject *parent = nullptr);

    QString defaultDocumentsPath() const;

    QString documentsPath() const;
    void setDocumentsPath(const QString &documentsPath);
    void unsetDocumentsPath();
    bool documentsPathIsDefault() const;

signals:

    void documentsPathChanged();

public slots:
};

#endif // DOCFILLSETTINGS_H

#ifndef DOCFILLNEWDOCUMENTCREATOR_H
#define DOCFILLNEWDOCUMENTCREATOR_H

#include <QObject>

class DocFillNewDocumentCreator : public QObject
{
    Q_OBJECT

    QString documentsPath_;
    Q_PROPERTY(QString documentsPath READ documentsPath WRITE setDocumentsPath NOTIFY documentsPathChanged)

public:
    explicit DocFillNewDocumentCreator(QObject *parent = nullptr);

    QString documentsPath() const;
    void setDocumentsPath(const QString &documentsPath);

signals:

    void documentsPathChanged();

public slots:
};

#endif // DOCFILLNEWDOCUMENTCREATOR_H

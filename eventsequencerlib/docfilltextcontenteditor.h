#ifndef DOCFILLTEXTCONTENTEDITOR_H
#define DOCFILLTEXTCONTENTEDITOR_H

#include "document.h"

#include <QObject>

class DocFillTextContentEditor : public QObject
{
    Q_OBJECT

    Document* document_ = nullptr;
    Q_PROPERTY(Document* document READ document WRITE setDocument NOTIFY documentChanged)

public:
    explicit DocFillTextContentEditor(QObject *parent = nullptr);

    Document *document() const;
    void setDocument(Document *document);
    void clearDocument();

signals:

    void documentChanged();

public slots:
};

#endif // DOCFILLTEXTCONTENTEDITOR_H

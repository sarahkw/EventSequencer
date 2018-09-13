#ifndef DOCFILLTEXTCONTENTEDITOR_H
#define DOCFILLTEXTCONTENTEDITOR_H

#include "document.h"

#include <QObject>

class DocFillTextContentEditor : public QObject
{
    Q_OBJECT

public:
    explicit DocFillTextContentEditor(QObject *parent = nullptr);

    Q_INVOKABLE void appendText(Document* document, QString text);

signals:

public slots:
};

#endif // DOCFILLTEXTCONTENTEDITOR_H

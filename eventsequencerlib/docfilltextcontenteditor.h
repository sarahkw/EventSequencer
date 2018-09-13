#ifndef DOCFILLTEXTCONTENTEDITOR_H
#define DOCFILLTEXTCONTENTEDITOR_H

#include "document.h"

#include <QObject>

class DocFillTextContentEditor : public QObject
{
    Q_OBJECT

public:
    explicit DocFillTextContentEditor(QObject *parent = nullptr);

    Q_INVOKABLE static void appendText(Document* document, QString text);

    Q_INVOKABLE static QString truncatePreview(Document* document, int cursorFrame);

signals:

public slots:
};

#endif // DOCFILLTEXTCONTENTEDITOR_H

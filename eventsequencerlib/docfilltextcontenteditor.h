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

private:
    static bool truncateInternal(Document* document, int cursorFrame, bool dryRun, QString* previewText);
public:

    Q_INVOKABLE static QVariantList truncatePreview(Document* document, int cursorFrame);
    Q_INVOKABLE static void truncate(Document* document, int cursorFrame);

signals:

public slots:
};

#endif // DOCFILLTEXTCONTENTEDITOR_H

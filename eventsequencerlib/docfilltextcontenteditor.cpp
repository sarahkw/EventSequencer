#include "docfilltextcontenteditor.h"

#include "docfillstructure.h"
#include "channel/textchannel.h"
#include "collides.h"

DocFillTextContentEditor::DocFillTextContentEditor(QObject *parent) : QObject(parent)
{

}

QVariantList DocFillTextContentEditor::appendText(Document *document, QString text, bool autoNewLines)
{
    Q_ASSERT(document);
    DocFillStructure dfstructure;
    const bool loadResult = dfstructure.load(*document);
    Q_ASSERT(loadResult);

    QString currentContent = dfstructure.textChannel->content();
    if (autoNewLines && !currentContent.endsWith("\n")) {
        currentContent.append("\n");
    }
    int rangeBegin = currentContent.size();
    currentContent.append(text);
    if (autoNewLines && !currentContent.endsWith("\n")) {
        currentContent.append("\n");
    }
    int rangeEnd = currentContent.size();
    dfstructure.textChannel->setContent(currentContent);
    document->setEndFrame(currentContent.size());
    return {rangeBegin, rangeEnd};
}

bool DocFillTextContentEditor::truncateInternal(Document *document, int cursorFrame, bool dryRun, QString *previewText)
{
    Q_ASSERT(document);
    DocFillStructure dfstructure;
    const bool loadResult = dfstructure.load(*document);
    Q_ASSERT(loadResult);

    if (cursorFrame < 0 || cursorFrame > document->endFrame()) {
        if (previewText) *previewText = "*ERROR* Cursor not within range of document";
        return false;
    }

    bool reject = false;

    // This'll do. We can't have /that/ many strips.
    for (Strip* s : document->strips()) {
        if (s->startFrame() > cursorFrame ||
                Collides::startAndLength(s->startFrame(), s->length(),
                                         cursorFrame, 1)) {
            reject = true;
            break;
        }
    }

    if (reject) {
        if (previewText) *previewText = "*ERROR* Cannot truncate through strips";
        return false;
    }

    if (previewText) {
        *previewText = dfstructure.textChannel->content().mid(cursorFrame);
    }

    if (!dryRun) {
        QString currentContent = dfstructure.textChannel->content();
        currentContent.resize(cursorFrame);
        dfstructure.textChannel->setContent(currentContent);
        document->setEndFrame(currentContent.size());
    }
    return true;
}

QVariantList DocFillTextContentEditor::truncatePreview(Document *document, int cursorFrame)
{
    QString previewText;
    if (truncateInternal(document, cursorFrame, true, &previewText)) {
        return {true, previewText};
    } else {
        return {false, previewText};
    }
}

void DocFillTextContentEditor::truncate(Document *document, int cursorFrame)
{
    truncateInternal(document, cursorFrame, false, nullptr);
}

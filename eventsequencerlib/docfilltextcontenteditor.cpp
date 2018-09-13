#include "docfilltextcontenteditor.h"

#include "channel/docfillchannel.h"
#include "channel/textchannel.h"
#include "collides.h"

DocFillTextContentEditor::DocFillTextContentEditor(QObject *parent) : QObject(parent)
{

}

void DocFillTextContentEditor::appendText(Document *document, QString text)
{
    Q_ASSERT(document);
    auto* docFillChannel = qobject_cast<channel::DocFillChannel*>(document->defaultProgramChannel());
    Q_ASSERT(docFillChannel);
    auto* textChannel = qobject_cast<channel::TextChannel*>(document->getChannelByIndex(docFillChannel->textChannel()));
    Q_ASSERT(textChannel);

    QString currentContent = textChannel->content();
    currentContent.append(text);
    textChannel->setContent(currentContent);
    document->setEndFrame(currentContent.size());
}

bool DocFillTextContentEditor::truncateInternal(Document *document, int cursorFrame, bool dryRun, QString *previewText)
{
    Q_ASSERT(document);
    auto* docFillChannel = qobject_cast<channel::DocFillChannel*>(document->defaultProgramChannel());
    Q_ASSERT(docFillChannel);
    auto* textChannel = qobject_cast<channel::TextChannel*>(document->getChannelByIndex(docFillChannel->textChannel()));
    Q_ASSERT(textChannel);

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
        *previewText = textChannel->content().mid(cursorFrame);
    }

    if (!dryRun) {
        QString currentContent = textChannel->content();
        currentContent.resize(cursorFrame);
        textChannel->setContent(currentContent);
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

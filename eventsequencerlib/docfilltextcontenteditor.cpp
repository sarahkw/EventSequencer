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

QString DocFillTextContentEditor::truncatePreview(Document *document, int cursorFrame)
{
    Q_ASSERT(document);
    auto* docFillChannel = qobject_cast<channel::DocFillChannel*>(document->defaultProgramChannel());
    Q_ASSERT(docFillChannel);
    auto* textChannel = qobject_cast<channel::TextChannel*>(document->getChannelByIndex(docFillChannel->textChannel()));
    Q_ASSERT(textChannel);

    if (cursorFrame < 0 || cursorFrame > document->endFrame()) {
        return "*ERROR* Cursor not within range of document";
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
        return "*ERROR* Cannot truncate where there are strips.";
    }

    return textChannel->content().mid(cursorFrame);
}

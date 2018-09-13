#include "docfilltextcontenteditor.h"

#include "channel/docfillchannel.h"
#include "channel/textchannel.h"

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

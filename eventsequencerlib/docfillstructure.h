#ifndef DOCFILLSTRUCTURE_H
#define DOCFILLSTRUCTURE_H

namespace channel {
class DocFillChannel;
class TextChannel;
class CollateChannel;
class ChannelBase;
}
class Document;

struct DocFillStructure
{
    bool load(Document& document);
    channel::DocFillChannel* programChannel = nullptr;
    channel::TextChannel* textChannel = nullptr;
    channel::CollateChannel* collateChannel = nullptr;
    channel::ChannelBase* stripSourceChannel = nullptr;
};

#endif // DOCFILLSTRUCTURE_H

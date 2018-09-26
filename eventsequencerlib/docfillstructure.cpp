#include "docfillstructure.h"

#include "document.h"
#include "channel/docfillchannel.h"
#include "channel/textchannel.h"
#include "channel/collatechannel.h"

bool DocFillStructure::load(Document &document)
{
    this->programChannel = qobject_cast<channel::DocFillChannel*>(document.defaultProgramChannel());
    if (!this->programChannel) return false;

    this->textChannel = qobject_cast<channel::TextChannel*>(document.getChannelByIndex(this->programChannel->textChannel()));
    if (!this->textChannel) return false;

    this->collateChannel = qobject_cast<channel::CollateChannel*>(document.getChannelByIndex(this->programChannel->resourceChannel()));
    if (!this->collateChannel) return false;

    this->stripSourceChannel = qobject_cast<channel::ChannelBase*>(document.getChannelByIndex(this->collateChannel->channel()));
    if (!this->stripSourceChannel) return false;

    return true;
}

#include "stripslist.h"

#include <concatiodevice.h>

#include <memory>

namespace playable {

StripsList::StripsList(QObject *parent) : PlayableBase(parent)
{
    QObject::connect(this, &StripsList::cursorFrameChanged,
                     this, &StripsList::updateCurrentStripsIfSelectionModeIsChannel);
}

QIODevice *StripsList::createPlayableDevice(const QAudioFormat& outputFormat)
{
    std::unique_ptr<ConcatIODevice> playingDevice(new ConcatIODevice);
    for (const Strip* s : stripsToPlay_) {
        if (!appendUrlToConcatIODevice(*playingDevice, s->resourceUrl(), outputFormat)) {
            return nullptr;
        }
    }
    return playingDevice.release();
}

namespace {
QString describeStrip(const Strip* s)
{
    return QString("%1 - %2").arg(s->startFrame()).arg(s->resourceUrl().toString());
}
}

void StripsList::updateCurrentStrips()
{
    QStringList sl;

    for (const Strip* s : stripsToPlay_) {
        s->disconnect(this);
    }
    stripsToPlay_.clear();

    const auto takeStrip = [this, &sl](Strip* s) {
        QObject::connect(s, &Strip::resourceUrlChanged,
                         this, &StripsList::updateCurrentStrips);
        stripsToPlay_.push_back(s);
        sl.push_back(describeStrip(s));
    };

    switch (selectionMode()) {
    case SelectionMode::UNSET:
        break;
    case SelectionMode::Strip:
        if (selectedStrip_ != nullptr) {
            takeStrip(selectedStrip_);
        }
        break;
    case SelectionMode::ChannelFromBegin:
        if (selectedChannel_ != nullptr) {
            for (Strip* s : selectedChannel_->strips()) {
                takeStrip(s);
            }
        }
        break;
    case SelectionMode::ChannelFromCursor:
        if (selectedChannel_ != nullptr) {
            auto strips = selectedChannel_->strips();
            for (auto iter = std::lower_bound(
                     strips.begin(), strips.end(), cursorFrame_,
                     [](Strip* a, int b) { return a->startFrame() < b; });
                 iter != strips.end(); ++iter) {
                takeStrip(*iter);
            }
        }
        break;
    case SelectionMode::ChannelOnCursor:
        if (selectedChannel_ != nullptr) {
            auto strips = selectedChannel_->strips();
            auto iter =
                std::lower_bound(strips.begin(), strips.end(),
                                 cursorFrame_, [](Strip* a, int b) {
                                     return a->startFrame() + a->length() <= b;
                                 });
            if (iter != strips.end() &&
                (*iter)->startFrame() <= cursorFrame_) {
                takeStrip(*iter);
            }
        }
        break;
    }

    currentStripsReport_ = sl.join("\n");
    emit currentStripsReportChanged();
}

void StripsList::updateCurrentStripsIfSelectionModeIsStrip()
{
    if (selectionMode() == SelectionMode::Strip) {
        updateCurrentStrips();
    }
}

void StripsList::updateCurrentStripsIfSelectionModeIsChannel()
{
    switch (selectionMode()) {
    case SelectionMode::ChannelFromBegin:
    case SelectionMode::ChannelFromCursor:
    case SelectionMode::ChannelOnCursor:
        updateCurrentStrips();
        break;
    case SelectionMode::UNSET:
    case SelectionMode::Strip:
        break;
    }
}

QString StripsList::currentStripsReport() const
{
    return currentStripsReport_;
}

StripsList::SelectionMode StripsList::selectionMode() const
{
    return selectionMode_;
}

void StripsList::setSelectionMode(const SelectionMode &selectionMode)
{
    if (selectionMode_ != selectionMode) {
        selectionMode_ = selectionMode;
        emit selectionModeChanged();
        updateCurrentStrips();
    }
}

channel::ChannelBase *StripsList::selectedChannel() const
{
    return selectedChannel_;
}

void StripsList::setSelectedChannel(channel::ChannelBase *selectedChannel)
{
    if (selectedChannel_ != selectedChannel) {

        if (selectedChannel_ != nullptr) {
            selectedChannel_->disconnect(this);
        }

        if (selectedChannel != nullptr) {
            QObject::connect(selectedChannel, &channel::ChannelBase::destroyed,
                             this, &StripsList::clearSelectedChannel);

            QObject::connect(selectedChannel, &channel::ChannelBase::stripsChanged,
                             this, &StripsList::updateCurrentStripsIfSelectionModeIsChannel);
        }

        selectedChannel_ = selectedChannel;
        emit selectedChannelChanged();

        updateCurrentStripsIfSelectionModeIsChannel();
    }
}

void StripsList::clearSelectedChannel()
{
    setSelectedChannel(nullptr);
}

Strip *StripsList::selectedStrip() const
{
    return selectedStrip_;
}

void StripsList::setSelectedStrip(Strip *selectedStrip)
{
    if (selectedStrip_ != selectedStrip) {
        if (selectedStrip_ != nullptr) {
            selectedStrip_->disconnect(this);
        }
        if (selectedStrip != nullptr) {
            QObject::connect(selectedStrip, &Strip::destroyed,
                             this, &StripsList::clearSelectedStrip);
        }

        selectedStrip_ = selectedStrip;
        emit selectedStripChanged();

        updateCurrentStripsIfSelectionModeIsStrip();
    }
}

void StripsList::clearSelectedStrip()
{
    setSelectedStrip(nullptr);
}

int StripsList::cursorFrame() const
{
    return cursorFrame_;
}

void StripsList::setCursorFrame(int cursorFrame)
{
    if (cursorFrame_ != cursorFrame) {
        cursorFrame_ = cursorFrame;
        emit cursorFrameChanged();
    }
}

} // namespace playable

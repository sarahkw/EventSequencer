#ifndef PLAYABLE_STRIPSLIST_H
#define PLAYABLE_STRIPSLIST_H

#include "playablebase.h"

#include <strip.h>
#include <channel/channelbase.h>

#include <QObject>

namespace playable {

// Use signals/slots to let ConcatIODevice let us know when frames are
// being played. The auto-disconnect feature will prevent calls after
// destruction.
class StripsListCallbackHelper : public QObject
{
    Q_OBJECT
    int frameNumber_ = 0;
public:
    StripsListCallbackHelper(int frameNumber) : frameNumber_(frameNumber) { }
    void playing()
    {
        emit playingFrame(frameNumber_);
    }
signals:
    void playingFrame(int frameNumber);
};

class StripsList : public PlayableBase
{
    Q_OBJECT
public:
    enum class SelectionMode {
        UNSET,
        Strip,
        ChannelFromBegin,
        ChannelFromCursor,
        ChannelOnCursor
    };
    Q_ENUM(SelectionMode)
private:
    SelectionMode selectionMode_ = SelectionMode::Strip;
    Strip* selectedStrip_ = nullptr;
    channel::ChannelBase* selectedChannel_ = nullptr;
    int cursorFrame_ = 0;

    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
    Q_PROPERTY(Strip* selectedStrip READ selectedStrip WRITE setSelectedStrip NOTIFY selectedStripChanged)
    Q_PROPERTY(channel::ChannelBase* selectedChannel READ selectedChannel WRITE setSelectedChannel NOTIFY selectedChannelChanged)
    Q_PROPERTY(int cursorFrame READ cursorFrame WRITE setCursorFrame NOTIFY cursorFrameChanged)

    void updateCurrentStrips();
    void updateCurrentStripsIfSelectionModeIsStrip();
    void updateCurrentStripsIfSelectionModeIsChannel();

    QString currentStripsReport_;
    Q_PROPERTY(QString currentStripsReport READ currentStripsReport NOTIFY currentStripsReportChanged)

    std::vector<const Strip*> stripsToPlay_;

public:
    explicit StripsList(QObject *parent = nullptr);

    QIODevice* createPlayableDevice(const QAudioFormat& outputFormat) override;

    SelectionMode selectionMode() const;
    void setSelectionMode(const SelectionMode &selectionMode);

    Strip *selectedStrip() const;
    void setSelectedStrip(Strip *selectedStrip);
    void clearSelectedStrip();

    channel::ChannelBase *selectedChannel() const;
    void setSelectedChannel(channel::ChannelBase *selectedChannel);
    void clearSelectedChannel();

    int cursorFrame() const;
    void setCursorFrame(int cursorFrame);

    QString currentStripsReport() const;

signals:

    void selectionModeChanged();
    void selectedStripChanged();
    void selectedChannelChanged();
    void cursorFrameChanged();

    void currentStripsReportChanged();

    void playingFrame(int frameNumber);

public slots:
};

} // namespace playable

#endif // PLAYABLE_STRIPSLIST_H

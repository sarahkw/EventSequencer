#ifndef PLAYERCONTROL_H
#define PLAYERCONTROL_H

#include "audiocontrol.h"
#include "strip.h"
#include "channel/channelbase.h"

class QAudioOutput;
class QIODevice;

class PlayerControl : public AudioControl
{
    Q_OBJECT

    QAudioOutput* audioOutput_ = nullptr;
    QString audioOutputReadyStatus_;
    void updateAudioObject() override;
    Q_PROPERTY(bool audioOutputReady          READ audioOutputReady       NOTIFY audioOutputReadyStatusChanged)
    Q_PROPERTY(QString audioOutputReadyStatus READ audioOutputReadyStatus NOTIFY audioOutputReadyStatusChanged)

    bool autoStopOnIdle_ = false;
    Q_PROPERTY(bool autoStopOnIdle READ autoStopOnIdle WRITE setAutoStopOnIdle NOTIFY autoStopOnIdleChanged)

    void updateAudioState();

public:
    enum class SelectionMode {
        Strip,
        ChannelFromBegin,
        ChannelFromCursor,
        ChannelOnCursor,
        SingleUrl
    };
    Q_ENUM(SelectionMode)
private:
    SelectionMode selectionMode_ = SelectionMode::Strip;
    Strip* selectedStrip_ = nullptr;
    channel::ChannelBase* selectedChannel_ = nullptr;
    QString singleUrl_;
    int cursorFrame_ = 0;
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
    Q_PROPERTY(Strip* selectedStrip READ selectedStrip WRITE setSelectedStrip NOTIFY selectedStripChanged)
    Q_PROPERTY(channel::ChannelBase* selectedChannel READ selectedChannel WRITE setSelectedChannel NOTIFY selectedChannelChanged)
    Q_PROPERTY(QString singleUrl READ singleUrl WRITE setSingleUrl NOTIFY singleUrlChanged)
    Q_PROPERTY(int cursorFrame READ cursorFrame WRITE setCursorFrame NOTIFY cursorFrameChanged)

    void updateCurrentStrips();
    void updateCurrentStripsIfSelectionModeIsStrip();
    void updateCurrentStripsIfSelectionModeIsChannel();

    QString currentStripsReport_;
    Q_PROPERTY(QString currentStripsReport READ currentStripsReport NOTIFY currentStripsReportChanged)

    std::vector<const Strip*> stripsToPlay_;
    QIODevice* playingDevice_ = nullptr;

public:
    explicit PlayerControl(QObject* parent = nullptr);
    ~PlayerControl() override;

    bool audioOutputReady() const;
    QString audioOutputReadyStatus() const;

    bool autoStopOnIdle() const;
    void setAutoStopOnIdle(bool autoStopOnIdle);

    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();

    SelectionMode selectionMode() const;
    void setSelectionMode(const SelectionMode &selectionMode);

    Strip *selectedStrip() const;
    void setSelectedStrip(Strip *selectedStrip);
    void clearSelectedStrip();

    channel::ChannelBase *selectedChannel() const;
    void setSelectedChannel(channel::ChannelBase *selectedChannel);
    void clearSelectedChannel();

    QString singleUrl() const;
    void setSingleUrl(const QString &singleUrl);

    int cursorFrame() const;
    void setCursorFrame(int cursorFrame);

    QString currentStripsReport() const;

signals:

    void audioOutputReadyStatusChanged();
    void autoStopOnIdleChanged();

    void selectionModeChanged();
    void selectedStripChanged();
    void selectedChannelChanged();
    void singleUrlChanged();
    void cursorFrameChanged();

    void currentStripsReportChanged();
};

#endif // PLAYERCONTROL_H

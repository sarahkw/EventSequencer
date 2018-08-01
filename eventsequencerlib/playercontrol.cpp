#include "playercontrol.h"

#include "audioformatholder.h"
#include "sessionaudio.h"

#include <QAudioOutput>

#include <QDebug>
#include <QBuffer>

void PlayerControl::play()
{
    if (audioFormatHolder_ == nullptr || audioOutput_ == nullptr) {
        qWarning() << "Not ready";
        return;
    }
    if (audioOutput_->state() != QAudio::StoppedState) {
        qWarning() << "Not stopped";
        return;
    }

    auto buf = new QBuffer(this); // TEMPORARY! Replace with real data.
    buf->open(QBuffer::ReadOnly);
    audioOutput_->start(buf);
    updateAudioState();
}

void PlayerControl::stop()
{
    if (audioOutput_ == nullptr) {
        return;
    }

    if (audioOutput_->state() != QAudio::StoppedState) {
        audioOutput_->stop();
    }
}

void PlayerControl::updateAudioObject()
{
    QStringList errors;
    if (audioFormatHolder_ == nullptr) {
        errors.push_back("Audio format missing");
    }
    if (sessionAudio_ == nullptr) {
        errors.push_back("Session audio missing");
    }

    if (audioOutput_ != nullptr) {
        stop();
        delete audioOutput_;
        audioOutput_ = nullptr;
        updateAudioState();
    }

    if (errors.empty()) {
        Q_ASSERT(audioOutput_ == nullptr);
        QAudioFormat qaf = audioFormatHolder_->toQAudioFormat();
        audioOutput_ = new QAudioOutput(sessionAudio_->selectedOutputDevice(), qaf);

        QObject::connect(audioOutput_, &QAudioOutput::stateChanged,
                         this, &PlayerControl::updateAudioState);

        if (!sessionAudio_->selectedOutputDevice().isFormatSupported(qaf)) {
            errors.push_back("WARN: Device does not support format");
        }
    }

    audioOutputReadyStatus_ = errors.join(", ");
    emit audioOutputReadyStatusChanged();
}

void PlayerControl::updateAudioState()
{
    if (audioOutput_ == nullptr) {
        setAudioState(AudioControl::AudioState::Unset);
        setError("");
        return;
    }

    setAudioState(audioOutput_->state());
    setError(audioOutput_->error());
}

namespace {
QString describeStrip(Strip* s)
{
    return QString("%1 - %2").arg(s->startFrame()).arg(s->resource()->identifier());
}
}

void PlayerControl::updateCurrentStrips()
{
    QStringList sl;

    switch (selectionMode()) {
    case SelectionMode::Strip:
        if (selectedStrip_ != nullptr) {
            sl.push_back(describeStrip(selectedStrip_));
        }
        break;
    case SelectionMode::Channel:
        if (selectedChannel_ != nullptr) {
            auto sset = selectedChannel_->stripSet();
            if (sset != nullptr) {
                for (auto& stripHolder : *sset) {
                    sl.push_back(describeStrip(stripHolder.strip));
                }
            }
        }
        break;
    }

    currentStripsReport_ = sl.join("\n");
    emit currentStripsReportChanged();
}

void PlayerControl::updateCurrentStripsIfSelectionModeIsStrip()
{
    if (selectionMode() == SelectionMode::Strip) {
        updateCurrentStrips();
    }
}

void PlayerControl::updateCurrentStripsIfSelectionModeIsChannel()
{
    if (selectionMode() == SelectionMode::Channel) {
        updateCurrentStrips();
    }
}

QString PlayerControl::currentStripsReport() const
{
    return currentStripsReport_;
}

PlayerControl::SelectionMode PlayerControl::selectionMode() const
{
    return selectionMode_;
}

void PlayerControl::setSelectionMode(const SelectionMode &selectionMode)
{
    if (selectionMode_ != selectionMode) {
        selectionMode_ = selectionMode;
        emit selectionModeChanged();
        updateCurrentStrips();
    }
}

channel::ChannelBase *PlayerControl::selectedChannel() const
{
    return selectedChannel_;
}

void PlayerControl::setSelectedChannel(channel::ChannelBase *selectedChannel)
{
    if (selectedChannel_ != selectedChannel) {

        if (selectedChannel_ != nullptr) {
            selectedChannel_->disconnect(this);
        }

        if (selectedChannel != nullptr) {
            QObject::connect(selectedChannel, &channel::ChannelBase::destroyed,
                             this, &PlayerControl::clearSelectedChannel);

            QObject::connect(selectedChannel, &channel::ChannelBase::stripSetChanged,
                             this, &PlayerControl::updateCurrentStripsIfSelectionModeIsChannel);
            QObject::connect(selectedChannel, &channel::ChannelBase::stripLocationChanged,
                             this, &PlayerControl::updateCurrentStripsIfSelectionModeIsChannel);
        }

        selectedChannel_ = selectedChannel;
        emit selectedChannelChanged();

        updateCurrentStripsIfSelectionModeIsChannel();
    }
}

void PlayerControl::clearSelectedChannel()
{
    setSelectedChannel(nullptr);
}

Strip *PlayerControl::selectedStrip() const
{
    return selectedStrip_;
}

void PlayerControl::setSelectedStrip(Strip *selectedStrip)
{
    if (selectedStrip_ != selectedStrip) {
        if (selectedStrip_ != nullptr) {
            selectedStrip_->disconnect(this);
        }
        if (selectedStrip != nullptr) {
            QObject::connect(selectedStrip, &Strip::destroyed,
                             this, &PlayerControl::clearSelectedStrip);
        }

        selectedStrip_ = selectedStrip;
        emit selectedStripChanged();

        updateCurrentStripsIfSelectionModeIsStrip();
    }
}

void PlayerControl::clearSelectedStrip()
{
    setSelectedStrip(nullptr);
}

PlayerControl::PlayerControl(QObject* parent) : AudioControl(parent)
{
    updateAudioObject(); // Initial update of ready status
}

PlayerControl::~PlayerControl()
{
    stop();

    if (audioOutput_ != nullptr) {
        delete audioOutput_;
    }
}

bool PlayerControl::audioOutputReady() const
{
    return audioOutput_ != nullptr;
}

QString PlayerControl::audioOutputReadyStatus() const
{
    return audioOutputReadyStatus_;
}

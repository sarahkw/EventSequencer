#include "playercontrol.h"

#include "audioformatholder.h"
#include "sessionaudio.h"
#include "managedresources.h"

#include <QAudioOutput>

#include <QDebug>

#include <memory>

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

    if (playable_ == nullptr) {
        setError("Nothing to play");
        return;
    }

    const auto format = audioOutput_->format();

    std::unique_ptr<QIODevice> playingDevice(playable_->createPlayableDevice(format));
    if (!playingDevice) {
        setError(playable_->error());
        return;
    }

    playingDevice->open(QIODevice::ReadOnly);

    // I have observed that if there are no samples to actually play,
    // then the audio state stays active and doesn't stop.
    {
        // TODO We should really ensure that there is at least enough
        //      bytes for a sample across all the channels. But we
        //      don't have a way to read a frame from an I/O device
        //      based on the audio format yet.
        char nop;
        if (playingDevice->peek(&nop, 1) != 1) {
            setError("No data to play");
            return;
        }
    }

    playingDevice_ = playingDevice.release();
    audioOutput_->start(playingDevice_);

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

    if (playingDevice_ != nullptr) {
        delete playingDevice_;
        playingDevice_ = nullptr;
    }

    setError("");
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
        updateAudioState(); // Initial audio state

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
        setAudioState(AudioControl::AudioState::Unconfigured);
        setError("");
        return;
    }

    // I used to check for UnderrunError but after reading some src I don't think
    // it's necessary, and could possibly be deterimental, as android backend
    // sets state before setting UnderrunError.
    if (autoStopOnIdle_ && audioOutput_->state() == QAudio::IdleState) {
        stop();

        // Calling stop() will change state to "stopped". Prevent the idle state
        // from showing up anywhere.
        return;
    }

    setAudioState(audioOutput_->state());
    setError(audioOutput_->error());
}

QObject *PlayerControl::playable() const
{
    return playable_;
}

void PlayerControl::setPlayable(QObject *playable)
{
    playable::PlayableBase* playableBase = qobject_cast<playable::PlayableBase*>(playable);
    if (playable_ != playableBase) {

        stop();

        if (playable_ != nullptr) {
            playable_->disconnect(this);
        }

        playable_ = playableBase;

        if (playable_ != nullptr) {
            QObject::connect(playable_, &QObject::destroyed,
                             this, &PlayerControl::clearPlayable);
        }

        emit playableChanged();
    }
    if (playable != nullptr && playableBase == nullptr) {
        qWarning("PlayerControl: playable is of wrong type");
        emit playableChanged(); // Make QML re-read to see that it's nullptr.
    }
}

void PlayerControl::clearPlayable()
{
    stop();
    playable_ = nullptr;
    emit playableChanged();
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


bool PlayerControl::autoStopOnIdle() const
{
    return autoStopOnIdle_;
}

void PlayerControl::setAutoStopOnIdle(bool autoStopOnIdle)
{
    if (autoStopOnIdle_ != autoStopOnIdle) {
        autoStopOnIdle_ = autoStopOnIdle;
        emit autoStopOnIdleChanged();
    }
}

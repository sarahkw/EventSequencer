#include "audiocontrol.h"

#include "audioformatholder.h"
#include "sessionaudio.h"

AudioControl::AudioControl(QObject *parent) : QObject(parent)
{
    QObject::connect(this, &AudioControl::audioFormatHolderChanged,
                     this, &AudioControl::updateAudioObject);
    QObject::connect(this, &AudioControl::sessionAudioChanged,
                     this, &AudioControl::updateAudioObject);
}

QVariant AudioControl::audioState() const
{
    return QVariant::fromValue(audioState_);
}

void AudioControl::setAudioState(AudioState audioState)
{
    audioState_ = audioState;
    emit audioStateChanged();
}

void AudioControl::setAudioState(QAudio::State audioState)
{
    switch (audioState) {
    case QAudio::ActiveState:      setAudioState(AudioState::Active     ) ; break;
    case QAudio::SuspendedState:   setAudioState(AudioState::Suspended  ) ; break;
    case QAudio::StoppedState:     setAudioState(AudioState::Stopped    ) ; break;
    case QAudio::IdleState:        setAudioState(AudioState::Idle       ) ; break;
    case QAudio::InterruptedState: setAudioState(AudioState::Interrupted) ; break;
    }
}

QString AudioControl::error() const
{
    return error_;
}

void AudioControl::setError(const QString &error)
{
    if (error_ != error) {
        error_ = error;
        emit errorChanged();
    }
}

void AudioControl::setError(QAudio::Error error)
{
    switch (error) {
    case QAudio::NoError:       setError(""); break;
    case QAudio::OpenError:     setError("OpenError"); break;
    case QAudio::IOError:       setError("IOError"); break;
    case QAudio::UnderrunError: setError("UnderrunError"); break;
    case QAudio::FatalError:    setError("FatalError"); break;
    }
}

QObject *AudioControl::audioFormatHolder() const
{
    return audioFormatHolder_;
}

void AudioControl::setAudioFormatHolder(QObject *audioFormatHolder)
{
    AudioFormatHolder* tmp = nullptr;
    if (audioFormatHolder != nullptr) {
        tmp = qobject_cast<AudioFormatHolder*>(audioFormatHolder);
        Q_ASSERT(tmp != nullptr);
    }

    if (audioFormatHolder_ != tmp) {

        if (audioFormatHolder_ != nullptr) {
            audioFormatHolder_->disconnect(this);
        }

        if (tmp != nullptr) {
            QObject::connect(tmp, &QObject::destroyed,
                             this, &AudioControl::clearAudioFormatHolder);
            QObject::connect(tmp, &AudioFormatHolder::audioFormatChanged,
                             this, &AudioControl::updateAudioObject);
        }

        audioFormatHolder_ = tmp;
        emit audioFormatHolderChanged();
    }
}

void AudioControl::clearAudioFormatHolder()
{
    setAudioFormatHolder(nullptr);
}

QObject *AudioControl::sessionAudio() const
{
    return sessionAudio_;
}

void AudioControl::setSessionAudio(QObject *sessionAudio)
{
    SessionAudio* tmp = nullptr;
    if (sessionAudio != nullptr) {
        tmp = qobject_cast<SessionAudio*>(sessionAudio);
        Q_ASSERT(tmp != nullptr);
    }
    
    if (sessionAudio_ != tmp) {

        if (sessionAudio_ != nullptr) {
            sessionAudio_->disconnect(this);
        }

        if (tmp != nullptr) {
            QObject::connect(tmp, &QObject::destroyed,
                             this, &AudioControl::clearSessionAudio);
        }

        sessionAudio_ = tmp;
        emit sessionAudioChanged();
    }
}

void AudioControl::clearSessionAudio()
{
    setSessionAudio(nullptr);
}

#include "audiocontrol.h"

#include "audioformatholder.h"
#include "sessionaudio.h"

AudioControl::AudioControl(QObject *parent) : QObject(parent)
{

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
                             this, &AudioControl::updateAudioInput);
        }

        audioFormatHolder_ = tmp;
        emit audioFormatHolderChanged();
    }
}

void AudioControl::clearAudioFormatHolder()
{
    if (audioFormatHolder_ != nullptr) {
        audioFormatHolder_->disconnect(this);
        audioFormatHolder_ = nullptr;
        emit audioFormatHolderChanged();
    }
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
    if (sessionAudio_ != nullptr) {
        sessionAudio_->disconnect(this);
        sessionAudio_ = nullptr;
        emit sessionAudioChanged();
    }
}

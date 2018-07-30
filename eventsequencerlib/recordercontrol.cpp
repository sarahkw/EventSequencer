#include "recordercontrol.h"

#include "audioformatholder.h"
#include "sessionaudio.h"

#include <QDebug>

QObject *RecorderControl::audioFormatHolder() const
{
    return audioFormatHolder_;
}

void RecorderControl::setAudioFormatHolder(QObject *audioFormatHolder)
{
    AudioFormatHolder* tmp = nullptr;
    if (audioFormatHolder != nullptr) {
        tmp = qobject_cast<AudioFormatHolder*>(audioFormatHolder);
        Q_ASSERT(tmp != nullptr);
    }

    if (audioFormatHolder_ != tmp) {
        audioFormatHolder_ = tmp;
        emit audioFormatHolderChanged();
    }
}

QObject *RecorderControl::sessionAudio() const
{
    return sessionAudio_;
}

void RecorderControl::setSessionAudio(QObject *sessionAudio)
{
    SessionAudio* tmp = nullptr;
    if (sessionAudio != nullptr) {
        tmp = qobject_cast<SessionAudio*>(sessionAudio);
        Q_ASSERT(tmp != nullptr);
    }
    
    if (sessionAudio_ != tmp) {
        sessionAudio_ = tmp;
        emit sessionAudioChanged();
    }
}

void RecorderControl::debug()
{
    qInfo() << audioFormatHolder_ << sessionAudio_;
}

RecorderControl::RecorderControl(QObject *parent) : QObject(parent)
{

}

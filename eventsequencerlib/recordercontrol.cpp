#include "recordercontrol.h"

#include "audioformatholder.h"
#include "sessionaudio.h"

#include <QDebug>

QObject *RecorderControl::audioFormat() const
{
    return audioFormat_;
}

void RecorderControl::setAudioFormat(QObject *audioFormat)
{
    AudioFormatHolder* tmp = nullptr;
    if (audioFormat != nullptr) {
        tmp = qobject_cast<AudioFormatHolder*>(audioFormat);
        Q_ASSERT(tmp != nullptr);
    }

    if (audioFormat_ != tmp) {
        audioFormat_ = tmp;
        emit audioFormatChanged();
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
    qInfo() << audioFormat_ << sessionAudio_;
}

RecorderControl::RecorderControl(QObject *parent) : QObject(parent)
{

}

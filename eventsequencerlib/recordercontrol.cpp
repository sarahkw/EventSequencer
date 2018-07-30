#include "recordercontrol.h"

#include "audioformatholder.h"
#include "sessionaudio.h"
#include "aufileheader.h"

#include <QDebug>
#include <QAudioInput>

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

        if (audioFormatHolder_ != nullptr) {
            audioFormatHolder_->disconnect(this);
        }

        if (tmp != nullptr) {
            QObject::connect(tmp, &QObject::destroyed,
                             this, &RecorderControl::clearAudioFormatHolder);
            QObject::connect(tmp, &AudioFormatHolder::audioFormatChanged,
                             this, &RecorderControl::updateAudioInput);
        }

        audioFormatHolder_ = tmp;
        emit audioFormatHolderChanged();
    }
}

void RecorderControl::clearAudioFormatHolder()
{
    if (audioFormatHolder_ != nullptr) {
        audioFormatHolder_->disconnect(this);
        audioFormatHolder_ = nullptr;
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

        if (sessionAudio_ != nullptr) {
            sessionAudio_->disconnect(this);
        }

        if (tmp != nullptr) {
            QObject::connect(tmp, &QObject::destroyed,
                             this, &RecorderControl::clearSessionAudio);
        }

        sessionAudio_ = tmp;
        emit sessionAudioChanged();
    }
}

void RecorderControl::clearSessionAudio()
{
    if (sessionAudio_ != nullptr) {
        sessionAudio_->disconnect(this);
        sessionAudio_ = nullptr;
        emit sessionAudioChanged();
    }
}

void RecorderControl::record()
{

}

void RecorderControl::stop()
{

}

void RecorderControl::debug()
{
    qInfo() << audioFormatHolder_ << sessionAudio_;
}

void RecorderControl::updateAudioInput()
{
    QStringList errors;
    if (audioFormatHolder_ == nullptr) {
        errors.push_back("Audio format missing");
    }
    if (sessionAudio_ == nullptr) {
        errors.push_back("Session audio missing");
    }
    if (audioFormatHolder_ != nullptr) {
        AuFileHeader afh;
        if (!afh.loadFormat(audioFormatHolder_->toQAudioFormat())) {
            errors.push_back("Format not supported by .au");
        }
    }

    if (audioInput_ != nullptr) {
        delete audioInput_;
        audioInput_ = nullptr;
    }

    if (errors.empty()) {
        Q_ASSERT(audioInput_ == nullptr);
        QAudioFormat qaf = audioFormatHolder_->toQAudioFormat();
        audioInput_ = new QAudioInput(sessionAudio_->selectedInputDevice(), qaf);

        if (!sessionAudio_->selectedInputDevice().isFormatSupported(qaf)) {
            errors.push_back("WARN: Device does not support format");
        }
    }

    audioInputReadyStatus_ = errors.join(", ");
    emit audioInputReadyStatusChanged();
}

RecorderControl::RecorderControl(QObject *parent) : QObject(parent)
{
    QObject::connect(this, &RecorderControl::audioFormatHolderChanged,
                     this, &RecorderControl::updateAudioInput);
    QObject::connect(this, &RecorderControl::sessionAudioChanged,
                     this, &RecorderControl::updateAudioInput);
}

RecorderControl::~RecorderControl()
{
    if (audioInput_ != nullptr) {
        delete audioInput_;
    }
}

bool RecorderControl::audioInputReady() const
{
    return audioInput_ != nullptr;
}

QString RecorderControl::audioInputReadyStatus() const
{
    return audioInputReadyStatus_;
}

#include "session.h"

#include "sessionaudio.h"

bool Session::wantAudio() const
{
    return wantAudio_;
}

void Session::setWantAudio(bool wantAudio)
{
    if (wantAudio_ != wantAudio) {
        wantAudio_ = wantAudio;

        if (wantAudio) {
            Q_ASSERT(sessionAudio_ == nullptr);
            sessionAudio_ = new SessionAudio;
        }

        emit wantAudioChanged();

        if (!wantAudio) {
            Q_ASSERT(sessionAudio_ != nullptr);
            delete sessionAudio_;
            sessionAudio_ = nullptr;
        }

        emit audioChanged();
    }
}

QObject *Session::audioQObject()
{
    return sessionAudio_;
}

SessionAudio *Session::audio()
{
    return sessionAudio_;
}

Session::Session(QObject *parent) : QObject(parent)
{

}

Session::~Session()
{
    if (sessionAudio_ != nullptr) {
        delete sessionAudio_;
    }
}

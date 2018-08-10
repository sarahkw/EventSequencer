#include "session.h"

#include "sessionaudio.h"

#include <QDebug>
#include <QElapsedTimer>

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
    return audio();
}

SessionAudio *Session::audio()
{
    if (sessionAudio_ == nullptr) {
        // This might be faster if we don't have to enumerate all the devices.
        // Print annoying message just to remind us why the UI freezes.
        QElapsedTimer timer;
        timer.start();
        setWantAudio(true);
        qInfo() << "Auto creation of SessionAudio took" << timer.elapsed();
    }
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

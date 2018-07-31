#include "recordercontrol.h"

#include "audioformatholder.h"
#include "sessionaudio.h"
#include "aufileheader.h"

#include <QDebug>
#include <QAudioInput>
#include <QFile>

#include <memory>

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

void RecorderControl::record(QString fileName)
{
    if (audioFormatHolder_ == nullptr || audioInput_ == nullptr) {
        qWarning() << "Not ready";
        return;
    }
    if (audioInput_->state() != QAudio::StoppedState) {
        qWarning() << "Not stopped";
    }

    std::unique_ptr<QFile> of(new QFile(fileName));

    if (!of->open(allowOverwrite() ? QFile::WriteOnly : QFile::NewOnly)) {
        setError(QString("Cannot open file: %1").arg(of->errorString()));
        return;
    }

    AuFileHeader afh;
    if (!afh.loadFormat(audioFormatHolder_->toQAudioFormat())) {
        qCritical() << "??? Format was good but now is not?";
        return;
    }

    if (!afh.writeFile(*of)) {
        setError(QString("Cannot write header: %1").arg(of->errorString()));
        return;
    }

    outputFile_ = of.release();

    audioInput_->start(outputFile_);
    updateAudioState();
}

void RecorderControl::stop()
{
    if (audioInput_ == nullptr) {
        return;
    }

    if (audioInput_->state() != QAudio::StoppedState) {
        audioInput_->stop();
    }

    if (outputFile_ != nullptr) {
        outputFile_->close();
        outputFile_ = nullptr;
    }
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
        stop();
        delete audioInput_;
        audioInput_ = nullptr;
        updateAudioState();
    }

    if (errors.empty()) {
        Q_ASSERT(audioInput_ == nullptr);
        QAudioFormat qaf = audioFormatHolder_->toQAudioFormat();
        audioInput_ = new QAudioInput(sessionAudio_->selectedInputDevice(), qaf);

        QObject::connect(audioInput_, &QAudioInput::stateChanged,
                         this, &RecorderControl::updateAudioState);

        if (!sessionAudio_->selectedInputDevice().isFormatSupported(qaf)) {
            errors.push_back("WARN: Device does not support format");
        }
    }

    audioInputReadyStatus_ = errors.join(", ");
    emit audioInputReadyStatusChanged();
}

bool RecorderControl::allowOverwrite() const
{
    return allowOverwrite_;
}

void RecorderControl::setAllowOverwrite(bool allowOverwrite)
{
    if (allowOverwrite_ != allowOverwrite) {
        allowOverwrite_ = allowOverwrite;
        emit allowOverwriteChanged();
    }
}

QVariant RecorderControl::audioState() const
{
    return QVariant::fromValue(audioState_);
}

void RecorderControl::updateAudioState()
{
    if (audioInput_ == nullptr) {
        audioState_ = AudioState::Unset;
        emit audioStateChanged();
        setError("");
        return;
    }

    switch (audioInput_->state()) {
    case QAudio::ActiveState:      audioState_ = AudioState::Active; break;
    case QAudio::SuspendedState:   audioState_ = AudioState::Suspended; break;
    case QAudio::StoppedState:     audioState_ = AudioState::Stopped; break;
    case QAudio::IdleState:        audioState_ = AudioState::Idle; break;
    case QAudio::InterruptedState: audioState_ = AudioState::Interrupted; break;
    }

    emit audioStateChanged();

    switch (audioInput_->error()) {
    case QAudio::NoError:       setError(""); break;
    case QAudio::OpenError:     setError("OpenError"); break;
    case QAudio::IOError:       setError("IOError"); break;
    case QAudio::UnderrunError: setError("UnderrunError"); break;
    case QAudio::FatalError:    setError("FatalError"); break;
    }
}

QString RecorderControl::error() const
{
    return error_;
}

void RecorderControl::setError(const QString &error)
{
    if (error_ != error) {
        error_ = error;
        emit errorChanged();
    }
}

RecorderControl::RecorderControl(QObject *parent) : QObject(parent)
{
    QObject::connect(this, &RecorderControl::audioFormatHolderChanged,
                     this, &RecorderControl::updateAudioInput);
    QObject::connect(this, &RecorderControl::sessionAudioChanged,
                     this, &RecorderControl::updateAudioInput);

    updateAudioInput(); // Initial update of ready status
}

RecorderControl::~RecorderControl()
{
    stop(); // Also deletes outputFile_.

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

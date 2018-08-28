#include "recordercontrol.h"

#include "endianmodifyingiodevice.h"
#include "audioformatholder.h"
#include "sessionaudio.h"
#include "aufileheader.h"
#include "managedresources.h"
#include "resourcemetadata.h"

#include <QDebug>
#include <QAudioInput>
#include <QFile>
#include <QFileInfo>

#include <memory>

QString RecorderControl::fileResourceDirectory() const
{
    return fileResourceDirectory_;
}

void RecorderControl::setFileResourceDirectory(const QString &fileResourceDirectory)
{
    if (fileResourceDirectory_ != fileResourceDirectory) {
        fileResourceDirectory_ = fileResourceDirectory;
        emit fileResourceDirectoryChanged();
    }
}

void RecorderControl::record(QUrl url)
{
    if (audioFormatHolder_ == nullptr || audioInput_ == nullptr) {
        qWarning() << "RecorderControl Not ready";
        return;
    }
    if (audioInput_->state() != QAudio::StoppedState) {
        qWarning() << "RecorderControl Not stopped";
        return;
    }

    QString fileName;
    if (!ManagedResources(fileResourceDirectory()).urlConvertToFilePath(url, &fileName)) {
        setError("Missing file resource directory");
        return;
    }

    std::shared_ptr<QFile> of(new QFile(fileName));

    if (!of->open(allowOverwrite() ? QFile::WriteOnly : QFile::NewOnly)) {
        setError(QString("Cannot open file: %1").arg(of->errorString()));
        return;
    }

    writingUrl_ = url;

    AuFileHeader afh;
    auto audioFormat = audioFormatHolder_->toQAudioFormat();
    if (!afh.loadFormat(audioFormat)) {
        qCritical() << "??? Format was good but now is not?";
        return;
    }

    if (!afh.writeFile(*of, ResourceMetaData::createFromNow())) {
        setError(QString("Cannot write header: %1").arg(of->errorString()));
        return;
    }

    auto* emiod = new EndianModifyingIODevice(
        of, unsigned(audioFormat.sampleSize() / 8),
        audioFormat.byteOrder() == QAudioFormat::BigEndian
            ? EndianModifyingIODevice::Big
            : EndianModifyingIODevice::Little,
        EndianModifyingIODevice::Big);

    const bool success = emiod->open(QIODevice::WriteOnly);
    Q_ASSERT(success); // This can't fail because inferior is already open

    audioInput_->start(emiod);
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
        delete outputFile_;
        outputFile_ = nullptr;
    }

    if (!writingUrl_.isEmpty()) {
        emit fileDone(writingUrl_);
        writingUrl_.clear();
    }
}

void RecorderControl::updateAudioObject()
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
        updateAudioState(); // Initial audio state

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

void RecorderControl::updateAudioState()
{
    if (audioInput_ == nullptr) {
        setAudioState(AudioControl::AudioState::Unconfigured);
        setError("");
        return;
    }

    setAudioState(audioInput_->state());
    setError(audioInput_->error());
}

RecorderControl::RecorderControl(QObject *parent) : AudioControl(parent)
{
    updateAudioObject(); // Initial update of ready status
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

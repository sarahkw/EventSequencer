#include "resourcecreatorcontrol.h"

#include <QDebug>

QObject *ResourceCreatorControl::audioRecorder()
{
    return &audioRecorder_;
}

ResourceCreatorControl::ResourceCreatorControl(QObject *parent) : QObject(parent)
{
    updateAudioInputs();
    QObject::connect(&audioRecorder_, &QAudioRecorder::availableAudioInputsChanged,
                     this, &ResourceCreatorControl::updateAudioInputs);
}

QString ResourceCreatorControl::audioInputDescription(const QString &name) const
{
    return audioRecorder_.audioInputDescription(name);
}

QStringList ResourceCreatorControl::supportedContainers() const
{
    return audioRecorder_.supportedContainers();
}

QString ResourceCreatorControl::containerDescription(const QString &format) const
{
    return audioRecorder_.containerDescription(format);
}

QStringList ResourceCreatorControl::supportedAudioCodecs() const
{
    return audioRecorder_.supportedAudioCodecs();
}

QString ResourceCreatorControl::audioCodecDescription(const QString &codecName) const
{
    return audioRecorder_.audioCodecDescription(codecName);
}

void ResourceCreatorControl::updateAudioInputs()
{
    audioInputs_ = audioRecorder_.audioInputs();
    emit audioInputsChanged();

    defaultAudioInputIndex_ = -1;
    auto needle = audioRecorder_.defaultAudioInput();
    auto where =
            std::find(audioInputs_.begin(), audioInputs_.end(),
                      needle);
    if (where != audioInputs_.end()) {
        defaultAudioInputIndex_ = where - audioInputs_.begin();
    }
    Q_ASSERT(defaultAudioInputIndex_ != -1); // XXX should soft assert instead
    emit defaultAudioInputIndexChanged();
}

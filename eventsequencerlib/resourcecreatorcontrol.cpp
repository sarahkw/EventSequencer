#include "resourcecreatorcontrol.h"

#include <QDebug>

ResourceCreatorControl::ResourceCreatorControl(QObject *parent) : QObject(parent)
{
    updateAudioInputs();
    QObject::connect(&audioRecorder_, &QAudioRecorder::availableAudioInputsChanged,
                     this, &ResourceCreatorControl::updateAudioInputs);
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

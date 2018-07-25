#include "resourcecreatorcontrol.h"

ResourceCreatorControl::ResourceCreatorControl(QObject *parent) : QObject(parent)
{
    QObject::connect(&audioRecorder_, &QAudioRecorder::availableAudioInputsChanged,
                     this, &ResourceCreatorControl::availableAudioInputsChanged);
}

QStringList ResourceCreatorControl::audioInputs() const
{
    return audioRecorder_.audioInputs();
}

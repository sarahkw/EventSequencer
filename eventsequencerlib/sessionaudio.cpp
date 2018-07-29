#include "sessionaudio.h"

#include "audioformat.h"

int SessionAudio::selectedOutputIndex() const
{
    return selectedOutputIndex_;
}

void SessionAudio::setSelectedOutputIndex(int selectedOutputIndex)
{
    if (selectedOutputIndex_ != selectedOutputIndex) {
        selectedOutputIndex_ = selectedOutputIndex;
        emit selectedOutputIndexChanged();

        if (selectedOutputIndex == 0) {
            selectedOutputDevice_ = QAudioDeviceInfo::defaultOutputDevice();
        } else {
            int idx = selectedOutputIndex - 1;
            Q_ASSERT(idx >= 0 && idx < outputDevices_.size());
            selectedOutputDevice_ = outputDevices_[idx];
        }
    }
}

int SessionAudio::selectedInputIndex() const
{
    return selectedInputIndex_;
}

void SessionAudio::setSelectedInputIndex(int selectedInputIndex)
{
    if (selectedInputIndex_ != selectedInputIndex) {
        selectedInputIndex_ = selectedInputIndex;
        emit selectedInputIndexChanged();

        if (selectedInputIndex == 0) {
            selectedInputDevice_ = QAudioDeviceInfo::defaultInputDevice();
        } else {
            int idx = selectedInputIndex - 1;
            Q_ASSERT(idx >= 0 && idx < inputDevices_.size());
            selectedInputDevice_ = inputDevices_[idx];
        }
    }
}

QStringList SessionAudio::inputDevicesModel() const
{
    return inputDevicesModel_;
}

QStringList SessionAudio::outputDevicesModel() const
{
    return outputDevicesModel_;
}

const QAudioDeviceInfo& SessionAudio::selectedInputDevice() const
{
    return selectedInputDevice_;
}

const QAudioDeviceInfo& SessionAudio::selectedOutputDevice() const
{
    return selectedOutputDevice_;
}

SessionAudio::SessionAudio()
{
    inputDevices_ = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    outputDevices_ = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);

    inputDevicesModel_.push_back("Default");
    for (QAudioDeviceInfo& adi : inputDevices_) {
        inputDevicesModel_.push_back(adi.deviceName());
    }

    outputDevicesModel_.push_back("Default");
    for (QAudioDeviceInfo& adi : outputDevices_) {
        outputDevicesModel_.push_back(adi.deviceName());
    }

    selectedInputDevice_ = QAudioDeviceInfo::defaultInputDevice();
    selectedOutputDevice_ = QAudioDeviceInfo::defaultOutputDevice();
}

void SessionAudio::inputPreferredFormat(AudioFormat &af)
{
    af.fromQAudioFormat(selectedInputDevice_.preferredFormat());
}

void SessionAudio::inputPreferredFormat(QObject *af)
{
    auto x = qobject_cast<AudioFormat*>(af);
    Q_ASSERT(x);
    inputPreferredFormat(*x);
}

void SessionAudio::outputPreferredFormat(AudioFormat &af)
{
    af.fromQAudioFormat(selectedOutputDevice_.preferredFormat());
}

void SessionAudio::outputPreferredFormat(QObject *af)
{
    auto x = qobject_cast<AudioFormat*>(af);
    Q_ASSERT(x);
    outputPreferredFormat(*x);
}

QString SessionAudio::testFormatSupport(AudioFormat &af)
{
    QAudioFormat qaf = af.toQAudioFormat();
    QString ret;
    ret += QString("Input supports format: %1\n").arg(selectedInputDevice_.isFormatSupported(qaf) ? "YES" : "NO");
    ret += QString("Output supports format: %1\n").arg(selectedOutputDevice_.isFormatSupported(qaf) ? "YES" : "NO");
    return ret;
}

QString SessionAudio::testFormatSupport(QObject *af)
{
    auto x = qobject_cast<AudioFormat*>(af);
    Q_ASSERT(x);
    return testFormatSupport(*x);
}

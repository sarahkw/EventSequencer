#ifndef SESSIONAUDIO_H
#define SESSIONAUDIO_H

#include <QAudioDeviceInfo>
#include <QObject>

class AudioFormat;

class SessionAudio : public QObject
{
    Q_OBJECT

    QList<QAudioDeviceInfo> inputDevices_;
    QList<QAudioDeviceInfo> outputDevices_;

    QStringList inputDevicesModel_;
    QStringList outputDevicesModel_;

    Q_PROPERTY(QStringList inputDevicesModel READ inputDevicesModel CONSTANT)
    Q_PROPERTY(QStringList outputDevicesModel READ outputDevicesModel CONSTANT)

    int selectedInputIndex_ = 0;
    int selectedOutputIndex_ = 0;

    Q_PROPERTY(int selectedInputIndex READ selectedInputIndex WRITE setSelectedInputIndex NOTIFY selectedInputIndexChanged)
    Q_PROPERTY(int selectedOutputIndex READ selectedOutputIndex WRITE setSelectedOutputIndex NOTIFY selectedOutputIndexChanged)

private:

    QAudioDeviceInfo selectedInputDevice_;
    QAudioDeviceInfo selectedOutputDevice_;

public:

    SessionAudio();

    void inputPreferredFormat(AudioFormat& af);
    Q_INVOKABLE void inputPreferredFormat(QObject* af);
    void outputPreferredFormat(AudioFormat& af);
    Q_INVOKABLE void outputPreferredFormat(QObject* af);

    int selectedInputIndex() const;
    void setSelectedInputIndex(int selectedInputIndex);

    int selectedOutputIndex() const;
    void setSelectedOutputIndex(int selectedOutputIndex);

    QStringList inputDevicesModel() const;

    QStringList outputDevicesModel() const;

    const QAudioDeviceInfo& selectedInputDevice() const;
    const QAudioDeviceInfo& selectedOutputDevice() const;

signals:
    void selectedInputIndexChanged();
    void selectedOutputIndexChanged();
};

#endif // SESSIONAUDIO_H

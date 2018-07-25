#ifndef RESOURCECREATORCONTROL_H
#define RESOURCECREATORCONTROL_H

#include <QObject>
#include <QAudioRecorder>

class ResourceCreatorControl : public QObject
{
    Q_OBJECT

    QAudioRecorder audioRecorder_;
    Q_PROPERTY(QObject* audioRecorder READ audioRecorder CONSTANT)

    QStringList audioInputs_;
    int defaultAudioInputIndex_ = -1;

    Q_PROPERTY(QStringList audioInputs MEMBER audioInputs_ NOTIFY audioInputsChanged)
    Q_PROPERTY(int defaultAudioInputIndex MEMBER defaultAudioInputIndex_ NOTIFY defaultAudioInputIndexChanged)

    QObject* audioRecorder();

public:

    enum AudioRecorderState {
        StoppedState = QAudioRecorder::StoppedState,
        RecordingState = QAudioRecorder::RecordingState,
        PausedState = QAudioRecorder::PausedState
    };
    Q_ENUMS(AudioRecorderState)

    explicit ResourceCreatorControl(QObject *parent = nullptr);

    // Proxy calls to QAudioRecorder. Need wrapper with Q_INVOKABLE.
    Q_INVOKABLE QString audioInputDescription(const QString &name) const;
    Q_INVOKABLE QStringList supportedContainers() const;
    Q_INVOKABLE QString containerDescription(const QString &format) const;
    Q_INVOKABLE QStringList supportedAudioCodecs() const;
    Q_INVOKABLE QString audioCodecDescription(const QString &codecName) const;
//    Q_INVOKABLE QList<int> supportedAudioSampleRates(const QAudioEncoderSettings &settings = QAudioEncoderSettings(),
//                                                     bool *continuous = nullptr) const;

    Q_INVOKABLE void configure(const QString& codecName, const QString& container);

signals:

    void audioInputsChanged();
    void defaultAudioInputIndexChanged();

public slots:

private slots:
    void updateAudioInputs();
};

Q_DECLARE_METATYPE(ResourceCreatorControl::AudioRecorderState)

#endif // RESOURCECREATORCONTROL_H

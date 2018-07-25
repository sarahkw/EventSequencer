#ifndef RESOURCECREATORCONTROL_H
#define RESOURCECREATORCONTROL_H

#include <QObject>
#include <QAudioRecorder>

class ResourceCreatorControl : public QObject
{
    Q_OBJECT

    QAudioRecorder audioRecorder_;

    QStringList audioInputs_;
    int defaultAudioInputIndex_ = -1;

    Q_PROPERTY(QStringList audioInputs MEMBER audioInputs_ NOTIFY audioInputsChanged)
    Q_PROPERTY(int defaultAudioInputIndex MEMBER defaultAudioInputIndex_ NOTIFY defaultAudioInputIndexChanged)

public:
    explicit ResourceCreatorControl(QObject *parent = nullptr);

signals:

    void audioInputsChanged();
    void defaultAudioInputIndexChanged();

public slots:

private slots:
    void updateAudioInputs();
};

#endif // RESOURCECREATORCONTROL_H

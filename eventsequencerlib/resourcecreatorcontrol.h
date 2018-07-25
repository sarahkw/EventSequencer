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
    explicit ResourceCreatorControl(QObject *parent = nullptr);

    Q_INVOKABLE QString audioInputDescription(const QString &name) const;

signals:

    void audioInputsChanged();
    void defaultAudioInputIndexChanged();

public slots:

private slots:
    void updateAudioInputs();
};

#endif // RESOURCECREATORCONTROL_H

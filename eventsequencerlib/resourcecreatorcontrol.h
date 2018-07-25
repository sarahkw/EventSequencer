#ifndef RESOURCECREATORCONTROL_H
#define RESOURCECREATORCONTROL_H

#include <QObject>
#include <QAudioRecorder>

class ResourceCreatorControl : public QObject
{
    Q_OBJECT

    QAudioRecorder audioRecorder_;

    Q_PROPERTY(QStringList audioInputs READ audioInputs NOTIFY availableAudioInputsChanged)

public:
    explicit ResourceCreatorControl(QObject *parent = nullptr);

    QStringList audioInputs() const;

signals:

    void availableAudioInputsChanged();

public slots:
};

#endif // RESOURCECREATORCONTROL_H

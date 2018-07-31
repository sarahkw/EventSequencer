#ifndef RECORDERCONTROL_H
#define RECORDERCONTROL_H

#include "audiocontrol.h"

#include <QObject>

class QAudioInput;
class QFile;

class RecorderControl : public AudioControl
{
    Q_OBJECT

private:

    QAudioInput* audioInput_ = nullptr;
    QString audioInputReadyStatus_;
    void updateAudioInput() override;
    Q_PROPERTY(bool audioInputReady          READ audioInputReady       NOTIFY audioInputReadyStatusChanged)
    Q_PROPERTY(QString audioInputReadyStatus READ audioInputReadyStatus NOTIFY audioInputReadyStatusChanged)

    QFile* outputFile_ = nullptr;

    bool allowOverwrite_ = false;
    Q_PROPERTY(bool allowOverwrite READ allowOverwrite WRITE setAllowOverwrite NOTIFY allowOverwriteChanged)

    QString writingPath_;

public:
    explicit RecorderControl(QObject *parent = nullptr);
    ~RecorderControl() override;

    bool audioInputReady() const;
    QString audioInputReadyStatus() const;

    Q_INVOKABLE void record(QString fileName);
    Q_INVOKABLE void stop();
    Q_INVOKABLE void debug();

private:
    void updateAudioState();
public:

    bool allowOverwrite() const;
    void setAllowOverwrite(bool allowOverwrite);

signals:

    void audioInputReadyStatusChanged();

    void allowOverwriteChanged();

    void fileDone(QString filePath, QString fileName);

public slots:
};

#endif // RECORDERCONTROL_H

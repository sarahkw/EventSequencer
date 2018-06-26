#ifndef STRIP_H
#define STRIP_H

#include <QObject>

class Strip : public QObject
{
    Q_OBJECT

    int startFrame_;
    int channel_;
    int length_;

    Q_PROPERTY(int startFrame READ startFrame WRITE setStartFrame NOTIFY startFrameChanged)
    Q_PROPERTY(int channel READ channel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)

public:
    explicit Strip(QObject *parent = nullptr);

    int startFrame() const;
    void setStartFrame(int startFrame);

    int channel() const;
    void setChannel(int channel);

    int length() const;
    void setLength(int length);

signals:

    void startFrameChanged();
    void channelChanged();
    void lengthChanged();

public slots:
};

Q_DECLARE_METATYPE(Strip*)

#endif // STRIP_H

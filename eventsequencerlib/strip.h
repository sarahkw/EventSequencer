#ifndef STRIP_H
#define STRIP_H

#include <QObject>

namespace pb {
class Strip;
}

class Strip : public QObject
{
    Q_OBJECT

    int channel_ = 0;
    int startFrame_ = 0;
    int length_ = 0;

    Q_PROPERTY(int channel READ channel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(int startFrame READ startFrame WRITE setStartFrame NOTIFY startFrameChanged)
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)

public:
    explicit Strip(QObject *parent = nullptr);

    void toPb(pb::Strip& pb) const;

    int channel() const;
    void setChannel(int channel);

    int startFrame() const;
    void setStartFrame(int startFrame);

    int length() const;
    void setLength(int length);

signals:

    void channelChanged();
    void startFrameChanged();
    void lengthChanged();

public slots:
};

Q_DECLARE_METATYPE(Strip*)

#endif // STRIP_H

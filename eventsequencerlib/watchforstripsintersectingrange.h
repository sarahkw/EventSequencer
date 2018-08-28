#ifndef WATCHFORSTRIPSINTERSECTINGRANGE_H
#define WATCHFORSTRIPSINTERSECTINGRANGE_H

#include "channel/channelbase.h"

#include <QObject>
#include <QVariantList>

class WatchForStripsIntersectingRange : public QObject
{
    Q_OBJECT

    channel::ChannelBase* channel_ = nullptr;
    int startFrame_ = 0;
    int length_ = 0;

    QVariantList strips_;

    Q_PROPERTY(QObject*      channel    READ channel    WRITE setChannel    NOTIFY channelChanged)
    Q_PROPERTY(int           startFrame READ startFrame WRITE setStartFrame NOTIFY startFrameChanged)
    Q_PROPERTY(int           length     READ length     WRITE setLength     NOTIFY lengthChanged)
    Q_PROPERTY(QVariantList  strips     READ strips                         NOTIFY stripsChanged)

    void updateStrips();

public:
    explicit WatchForStripsIntersectingRange(QObject *parent = nullptr);

    QObject *channel() const;
    void setChannel(QObject *channel);
    void destroyingChannel();

    int startFrame() const;
    void setStartFrame(int startFrame);

    int length() const;
    void setLength(int length);

    QVariantList strips() const;

    Q_INVOKABLE bool isStripInView(Strip* s) const;

signals:

    void channelChanged();
    void startFrameChanged();
    void lengthChanged();
    void stripsChanged();

public slots:
};

#endif // WATCHFORSTRIPSINTERSECTINGRANGE_H

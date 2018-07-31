#ifndef DOCUMENTSTRIPSONCHANNEL_H
#define DOCUMENTSTRIPSONCHANNEL_H

#include <QObject>
#include <set>

class Strip;

class DocumentStripsOnChannel : public QObject
{
    Q_OBJECT

    std::map<int, std::set<Strip*>> data_;

public:
    explicit DocumentStripsOnChannel(QObject *parent = nullptr);

    const std::set<Strip*>* stripsForChannel(int channel) const;

signals:

    void channelStripSetChanged(int channel);
    void channelStripLocationChanged(int channel, Strip* whichStrip);

public slots:
    void stripAfterPlaced(Strip* strip);
    void stripBeforeDelete(Strip* strip);
    void stripMoved(Strip* strip,
                    int previousChannel,
                    int previousStartFrame,
                    int previousLength);
};

#endif // DOCUMENTSTRIPSONCHANNEL_H

#ifndef CHANNELTYPE_H
#define CHANNELTYPE_H

#include <QObject>

namespace channel {

// QObject to expose enum to QML.
class ChannelType : public QObject {
    Q_OBJECT
public:
    ChannelType(QObject* parent);

    enum Enum
    {
        BadClock,
        BadJs,
        Text,
        BadAudio,
        Label
    };
    Q_ENUM(Enum)
};

} // namespace channel

Q_DECLARE_METATYPE(channel::ChannelType::Enum)

#endif // CHANNELTYPE_H

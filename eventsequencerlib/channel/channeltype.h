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
        UNSET = 0,     // 0 for protobuf default value.
        BadClock = 1,
        BadJs = 2,
        Text = 3,
        Audio = 4,
        Label = 5,
        Collate = 6,
        Playlist = 7,
        Span = 8,
        DocFill = 9,

        MAX = DocFill
    };
    Q_ENUM(Enum)

    static int toInt(Enum value);
    static Enum fromInt(int value);
};

} // namespace channel

Q_DECLARE_METATYPE(channel::ChannelType::Enum)

#endif // CHANNELTYPE_H

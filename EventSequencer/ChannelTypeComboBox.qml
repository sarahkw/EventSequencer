import QtQuick 2.0
import QtQuick.Controls 2.2
import eventsequencer 1.0 as ES

ComboBox {
    property var currentChannelType
    signal updateChannelType(var newChannelType)

    readonly property var modelUtil: (function () { return {
        model: [
            "",
            "BadClock",
            "BadJs",
            "Text",
            "Audio",
            "Label",
            "Collate",
            "Playlist",
            "Span",
            "DocFill"
        ],
        enumToIndex: function (v) {
            switch (v) {
            case ES.ChannelType.BadClock: return 1;
            case ES.ChannelType.BadJs: return 2;
            case ES.ChannelType.Text: return 3;
            case ES.ChannelType.Audio: return 4;
            case ES.ChannelType.Label: return 5;
            case ES.ChannelType.Collate: return 6;
            case ES.ChannelType.Playlist: return 7;
            case ES.ChannelType.Span: return 8;
            case ES.ChannelType.DocFill: return 9;
            }
        },
        indexToEnum: function (v) {
            switch (v) {
            case 1: return ES.ChannelType.BadClock;
            case 2: return ES.ChannelType.BadJs;
            case 3: return ES.ChannelType.Text;
            case 4: return ES.ChannelType.Audio;
            case 5: return ES.ChannelType.Label;
            case 6: return ES.ChannelType.Collate;
            case 7: return ES.ChannelType.Playlist;
            case 8: return ES.ChannelType.Span;
            case 9: return ES.ChannelType.DocFill;
            }
        },
    }})()

    model: modelUtil.model
    currentIndex: currentChannelType !== null ? modelUtil.enumToIndex(currentChannelType) : 0

    onActivated: {
        if (index === 0) {
            updateChannelType(null)
        } else {
            updateChannelType(modelUtil.indexToEnum(index))
        }
    }
}

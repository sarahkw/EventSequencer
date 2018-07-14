import QtQuick 2.0

import eventsequencer 1.0 as ES

Item {
    BadClock {
        id: badClock
    }
    BadJs {
        id: badJs
    }
    TextControl {
        id: text
    }
    BadAudioControl {
        id: badAudio
    }
    LabelControl {
        id: label
    }

    function resolve(type) {
        switch (type) {
        case ES.ChannelType.BadClock: return badClock
        case ES.ChannelType.BadJs: return badJs
        case ES.ChannelType.Text: return text
        case ES.ChannelType.BadAudio: return badAudio
        case ES.ChannelType.Label: return label
        }
        console.assert(false, "Unable to resolve type")
    }
}

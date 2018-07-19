import QtQuick 2.0

import eventsequencer 1.0 as ES

Item {
    BadClockControl {
        id: badClock
    }
    BadJsControl {
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
    CollateControl {
        id: collate
    }

    function resolve(type) {
        switch (type) {
        case ES.ChannelType.BadClock: return badClock
        case ES.ChannelType.BadJs: return badJs
        case ES.ChannelType.Text: return text
        case ES.ChannelType.BadAudio: return badAudio
        case ES.ChannelType.Label: return label
        case ES.ChannelType.Collate: return collate
        }
        console.assert(false, "Unable to resolve type")
    }
}

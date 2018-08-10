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
    AudioControl {
        id: audio
    }
    LabelControl {
        id: label
    }
    CollateControl {
        id: collate
    }
    PlaylistControl {
        id: playlist
    }
    SpanControl {
        id: span
    }
    DocFillControl {
        id: docFill
    }

    function resolve(type) {
        switch (type) {
        case ES.ChannelType.BadClock: return badClock
        case ES.ChannelType.BadJs: return badJs
        case ES.ChannelType.Text: return text
        case ES.ChannelType.Audio: return audio
        case ES.ChannelType.Label: return label
        case ES.ChannelType.Collate: return collate
        case ES.ChannelType.Playlist: return playlist
        case ES.ChannelType.Span: return span
        case ES.ChannelType.DocFill: return docFill
        }
        console.assert(false, "Unable to resolve type")
    }
}

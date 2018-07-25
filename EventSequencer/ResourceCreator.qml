import QtQuick 2.0
import QtQuick.Window 2.3

import eventsequencer 1.0 as ES

Window {
    width: 400
    height: 600
    title: "Resource Creator"
    flags: Qt.Dialog

    ES.ResourceCreatorControl {
        id: rcControl
    }
}

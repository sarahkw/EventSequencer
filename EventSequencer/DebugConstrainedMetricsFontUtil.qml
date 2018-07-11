import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.2

import eventsequencer 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: "DebugConstrainedMetricsFontUtil"

    property int gridSize: mf.fontCharacterWidth(sbox.value, sbLetterSpacing.value)

    ConstrainedMetricsFontUtil {
        id: mf
    }

    Repeater {
        anchors.fill: parent
        model: Math.ceil(width / gridSize)
        Rectangle {
            width: 1
            x: index * gridSize
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            color: "black"
        }
    }

    Column {
        Row {
            Label {
                text: "pixelSize"
                anchors.verticalCenter: parent.verticalCenter
            }
            SpinBox {
                id: sbox
                value: 12
            }
        }
        Row {
            Label {
                text: "letterSpacing"
                anchors.verticalCenter: parent.verticalCenter
            }
            SpinBox {
                id: sbLetterSpacing
                from: -100
                value: 0
            }
        }
        Text {
            text: "These are some words that must line up."
            font: mf.makeFont(sbox.value, sbLetterSpacing.value)
        }
    }
}

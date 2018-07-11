import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.2
import Qt.labs.platform 1.0 as Qlp

import eventsequencer 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: "DebugConstrainedMetricsFontUtil"

    property font myFont: mf.makeFont(sbox.value, sbLetterSpacing.value)
    property int gridSize: mf.fontCharacterWidth(txtLineMeUp.font)

    Qlp.FontDialog {
        id: fontDialog
        options: Qlp.FontDialog.MonospacedFonts
    }

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
        Item {
            width: controls.width
            height: controls.height

            Rectangle {
                color: "white"
                anchors.fill: parent
            }

            Grid {
                id: controls
                spacing: 5
                columns: 2
                verticalItemAlignment: Grid.AlignVCenter
                Button {
                    text: "Font Dialog"
                    onClicked: {
                        fontDialog.open()
                    }
                }
                Label {
                    text: fontDialog.font
                }
                Label {
                    text: "pixelSize"
                }
                SpinBox {
                    id: sbox
                    value: 12
                }
                Label {
                    text: "letterSpacing"
                }
                SpinBox {
                    id: sbLetterSpacing
                    from: -100
                    value: 0
                }
            }
        }
        Text {
            id: txtLineMeUp
            text: "These are some words that must line up."
            font: myFont
        }
    }
}

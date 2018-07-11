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

    property font myFont: mf.makeUniformPixelWidth(mf.defaultFont())
    property int gridSize: mf.fontCharacterWidth(myFont)

    Qlp.FontDialog {
        id: fontDialog
        options: Qlp.FontDialog.MonospacedFonts
        onAccepted: {
            myFont = mf.makeUniformPixelWidth(font)
        }
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
                    text: myFont
                }
                Label {
                    text: "pixelSize"
                }
                SpinBox {
                    id: sbox
                    value: 12
                    onValueChanged: {
                        myFont.pixelSize = value
                    }
                }
                Label {
                    text: "letterSpacing"
                }
                SpinBox {
                    id: sbLetterSpacing
                    from: -100
                    value: 0
                    onValueChanged: {
                        myFont.letterSpacing = value
                    }
                }
            }
        }
        Text {
            id: txtLineMeUp
            text: "These letters must line up with the grid."
            font: myFont
        }
    }
}

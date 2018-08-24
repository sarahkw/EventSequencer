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

    property font myFont: mfAuto.builtFont
    property int gridSize: ctrl_constrainByWidthValue.value

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

    Row {
        id: ctrlRow
        y: 10
        x: 10
        spacing: 20

        Item {
            id: fontSelect
            width: fontSelectInternal.width
            height: fontSelectInternal.height

            Qlp.FontDialog {
                id: fontDialog
                options: Qlp.FontDialog.MonospacedFonts
                onAccepted: {
                    mfAuto.baseFont = font
                }
            }

            Rectangle {
                color: "white"
                border.width: 1
                anchors.fill: parent
                anchors.margins: -5
            }

            Grid {
                id: fontSelectInternal
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
            }
        }

        Item {
            width: autoControls.width
            height: autoControls.height

            ConstrainedMetricsFontUtil {
                id: mfAuto
                constrainByWidthEnabled: ctrl_constrainByWidthEnabled.checked
                constrainByWidthValue: ctrl_constrainByWidthValue.value
                addLetterSpacingToMatchWidth: ctrl_addLetterSpacingToMatchWidth.checked
                constrainByHeightEnabled: ctrl_constrainByHeightEnabled.checked
                constrainByHeightValue: ctrl_constrainByHeightValue.value
            }

            Rectangle {
                color: "white"
                border.width: 1
                anchors.fill: parent
                anchors.margins: -5
            }

            Column {
                id: autoControls
                Grid {
                    spacing: 5
                    columns: 2
                    verticalItemAlignment: Grid.AlignVCenter
                    Label {
                        text: "constrainByWidthEnabled"
                    }
                    CheckBox {
                        id: ctrl_constrainByWidthEnabled
                    }
                    Label {
                        text: "constrainByWidthValue"
                    }
                    SpinBox {
                        id: ctrl_constrainByWidthValue
                        value: 12
                    }
                    Label {
                        text: "constrainByHeightEnabled"
                    }
                    CheckBox {
                        id: ctrl_constrainByHeightEnabled
                    }
                    Label {
                        text: "constrainByHeightValue"
                    }
                    SpinBox {
                        id: ctrl_constrainByHeightValue
                        value: 20
                    }
                    Label {
                        text: "addLetterSpacingToMatchWidth"
                    }
                    CheckBox {
                        id: ctrl_addLetterSpacingToMatchWidth
                        text: "Value"
                        checked: true
                    }
                    Label {
                        text: "builtFontFailedToMeetConstraints"
                    }
                    CheckBox {
                        id: ctrl_builtFontFailedToMeetConstraints
                        text: "Value"
                        enabled: false
                        checked: mfAuto.builtFontFailedToMeetConstraints
                    }
                    Label {
                        text: "builtFontAddedSpacing"
                    }
                    Label {
                        text: mfAuto.builtFontAddedSpacing
                    }
                    Label {
                        text: "builtFontWidth"
                    }
                    Label {
                        text: mfAuto.builtFontWidth
                    }
                }
            }
        }
    }
    Text {
        anchors.top: ctrlRow.bottom
        anchors.topMargin: 10

        id: txtLineMeUp
        text: "These letters must line up with the grid."
        font: myFont

        Rectangle {
            border.width: 1
            anchors.left: txtLineMeUp.left
            anchors.top: txtLineMeUp.top
            anchors.right: txtLineMeUp.right
            height: mfAuto.fontHeight(myFont)
            color: "#00000000"
        }
    }
}

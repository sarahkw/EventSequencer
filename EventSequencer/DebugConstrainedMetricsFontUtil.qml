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

    property font myFont: mfManual.makeUniformPixelWidth(mfManual.defaultFont())
    property int gridSize: mfManual.fontCharacterWidth(myFont)

    ConstrainedMetricsFontUtil {
        id: mfManual
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
                    myFont = mfManual.makeUniformPixelWidth(font)
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
                Label {
                    text: "fontBriefInformation"
                }
                Label {
                    text: mfManual.fontBriefInformation(myFont)
                }
                Item { width: 1; height: 1 }
                Button {
                    text: "Dump Font Information"
                    onClicked: mfManual.dumpFontInformation(myFont)
                }
            }
        }

        Item {
            width: basicControls.width
            height: basicControls.height

            Rectangle {
                color: "white"
                border.width: 1
                anchors.fill: parent
                anchors.margins: -5
            }

            Column {
                id: basicControls
                Grid {
                    spacing: 5
                    columns: 2
                    verticalItemAlignment: Grid.AlignVCenter
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
        }

        Item {
            width: autoControls.width
            height: autoControls.height

            ConstrainedMetricsFontUtil {
                id: mfAuto
                constrainByWidthValue: ctrl_constrainByWidthValue.value
                addLetterSpacingToMatchWidth: ctrl_addLetterSpacingToMatchWidth.checked
                constainByHeightEnabled: ctrl_constainByHeightEnabled.checked
                constrainByHeightValue: ctrl_constrainByHeightValue.value
                baseFont: myFont
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
                        text: "constrainByWidthValue"
                    }
                    SpinBox {
                        id: ctrl_constrainByWidthValue
                        value: 12
                    }
                    Label {
                        text: "constainByHeightEnabled"
                    }
                    CheckBox {
                        id: ctrl_constainByHeightEnabled
                    }
                    Label {
                        text: "constrainByHeightValue"
                    }
                    SpinBox {
                        id: ctrl_constrainByHeightValue
                    }
                    Label {
                        text: "addLetterSpacingToMatchWidth"
                    }
                    CheckBox {
                        id: ctrl_addLetterSpacingToMatchWidth
                        text: "Value"
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
                }
                Button {
                    anchors.right: parent.right
                    text: "Use Auto Font (One-Shot)"
                    onClicked: {
                        myFont = mfAuto.buildFont()
                        gridSize = ctrl_constrainByWidthValue.value
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
            height: mfManual.fontHeight(myFont)
            color: "#00000000"
        }
    }
}

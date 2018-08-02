import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.2

import eventsequencer 1.0
import QtQuick.Layouts 1.3

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Document View")
    flags: Qt.Dialog

    property bool destroyOnHide: false
    onVisibleChanged: {
        if (destroyOnHide && visible === false) {
            destroy()
        }
    }

    property string crazyText: "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.

Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

    ConstrainedMetricsFontUtil {
        id: cmfu
        constrainByWidthValue: 10
    }

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        Flow {
            id: flow1
            width: 400
            height: 400
            Layout.fillWidth: true

            Row {
                id: row

                Label {
                    id: label
                    text: qsTr("Text Channel")
                }

                TextField {
                    id: textField
                    text: qsTr("Text Field")
                }
            }

            Row {
                id: row1

                Label {
                    id: label1
                    text: qsTr("Render Channel")
                }

                TextField {
                    id: textField1
                    text: qsTr("Text Field")
                }
            }
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            clip: true
            ListView {
                id: lview
                model: wwtt
                delegate: Item {
                    height: cmfu.builtFontHeight * 2
                    Text {
                        text: modelData
                        font: cmfu.builtFont
                    }
                    Rectangle {
                        y: cmfu.builtFontHeight
                        height: cmfu.builtFontHeight
                        width: modelData.length * cmfu.constrainByWidthValue
                        color: "pink"
                    }
                }
            }
        }
    }

    WordWrappedTextTrack {
        id: wwtt
        width: lview.width - cmfu.constrainByWidthValue /*For wrapped space or NewLine*/
        text: crazyText
        font: cmfu.builtFont
    }

}

/*##^## Designer {
    D{i:8;anchors_height:100;anchors_width:100;anchors_x:5;anchors_y:5}
}
 ##^##*/

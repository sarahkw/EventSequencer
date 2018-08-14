import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

Window {
    visible: true
    width: 540
    height: 720
    title: "EvSeq Program Launcher"

    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: parent.width / 10
        anchors.rightMargin: parent.width / 10
        anchors.top: parent.top
        anchors.topMargin: parent.height / 20
        anchors.bottom: parent.bottom
        spacing: parent.height / 20
        Text {
            Layout.fillWidth: true
            text: qsTr("EvSeq Program Launcher")
            font.family: "Courier"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap

            font.pointSize: 18
        }

        RowLayout {
            Layout.fillWidth: true
            TextField {
                Layout.fillWidth: true
            }
            Button {
                text: "Browse"
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Text {
                Layout.fillWidth: true
                text: "Programs"
                font.family: "Courier"
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: 12
            }
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                ListView {
                    spacing: 5
                    model: [
                        "DocFill on 0",
                        "DocFill on 1",
                        "DocFill on 2"
                    ]

                    delegate: Button {
                        text: modelData
                    }

                }
            }
        }
    }
}

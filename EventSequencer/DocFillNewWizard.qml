import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

Rectangle {
    color: "whitesmoke"
    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: 5

        RowLayout {
            Layout.rightMargin: 5
            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: 12
                text: "New Document"
            }
            Button {
                text: "Cancel"
                onClicked: closeFn()
            }
            Button {
                text: "Back"
            }
            Button {
                text: "Next"
            }
        }

        RowLayout {
            Layout.leftMargin: 5
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                Layout.alignment: Qt.AlignTop
                Label {
                    text: "1. Content"
                    font.bold: true
                }
                Label {
                    text: "2. Audio"
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: 5
                Text {
                    text: "New Wizard"
                }
            }
        }
    }
}

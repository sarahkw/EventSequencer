import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "Control/" as Ctrl

GroupBox {
    id: root
    anchors.left: parent.left
    anchors.right: parent.right

    signal msgbox(string message)

    title: "Developer Options (no auto-save)"
    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right

        Ctrl.Resolver {
            id: controlResolver
        }

        Text {
            Layout.fillWidth: true
            wrapMode: Text.Wrap
            text: "<b>WARNING</b> These options will allow you to make changes that cause malfunction, whether it happens now or in future versions of the application. If you wish to experiment, make a test file."
        }
        RowLayout {
            Layout.fillWidth: true
            ComboBox {
                Layout.fillWidth: true
                id: cboxType
                model: [
                    "",
                    "Selected Strip",
                    "Channel",
                    "Document",
                    "File",
                    "Session",
                ]
            }
            TextField {
                visible: cboxType.currentIndex === 2
                inputMethodHints: Qt.ImhDigitsOnly
                implicitWidth: 75
            }
        }
        Loader {
            Layout.fillWidth: true
            id: propertiesLoader
            source: {
                switch (cboxType.currentIndex) {
                case 3: return "PropertiesDocument.qml"
                case 4: return "PropertiesFile.qml"
                case 5: return "PropertiesSession.qml"
                }
                if (cboxType.currentIndex === 1 &&
                        selectedCppStrip !== null) {
                    return "PropertiesStrip.qml"
                }
                return ""
            }
        }
        Connections {
            target: propertiesLoader.item
            ignoreUnknownSignals: true
            onMsgbox: msgbox(message)
        }
    }
}

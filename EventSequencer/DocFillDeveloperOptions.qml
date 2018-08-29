import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "Control/" as Ctrl

import eventsequencer 1.0 as ES

GroupBox {
    id: root
    anchors.left: parent.left
    anchors.right: parent.right

    signal msgbox(string message)

    title: "Developer Options (no auto-save)"
    ColumnLayout {
        id: clayout
        anchors.left: parent.left
        anchors.right: parent.right

        Ctrl.Resolver {
            id: controlResolver
        }

        property ES.WaitFor waitForChannel: document.waitForChannelIndex(ES.ChannelIndexFactory.makeFromPathString(txtChannelPathString.myChannelString))

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

                Component {
                    id: blankComponent
                    Item { }
                }
                onCurrentIndexChanged: {
                    propertiesLoader.sourceComponent = undefined
                    switch (currentIndex) {
                    case 2: propertiesLoader.source = "PropertiesChannel.qml"; return
                    case 3: propertiesLoader.source = "PropertiesDocument.qml"; return
                    case 4: propertiesLoader.source = "PropertiesFile.qml"; return
                    case 5: propertiesLoader.source = "PropertiesSession.qml"; return
                    }
                    if (currentIndex === 1 &&
                            selectedCppStrip !== null) {
                        propertiesLoader.source = "PropertiesStrip.qml"
                        return
                    }
                    propertiesLoader.source = ""
                    propertiesLoader.sourceComponent = blankComponent
                }
                property var waitForChange: selectedCppStrip
                onWaitForChangeChanged: {
                    if (currentIndex == 1) {
                        onCurrentIndexChanged()
                    }
                }
            }
            TextField {
                id: txtChannelPathString
                visible: cboxType.currentIndex === 2
                inputMethodHints: Qt.ImhDigitsOnly
                implicitWidth: 75
                selectByMouse: true
                text: "0"
                property string myChannelString: ""
                onEditingFinished: {
                    myChannelString = text
                    focus = false
                }
            }
        }

        Loader {
            Layout.fillWidth: true
            id: propertiesLoader
            property var sourceCppChannel: clayout.waitForChannel.result
        }
        Connections {
            target: propertiesLoader.item
            ignoreUnknownSignals: true
            onMsgbox: msgbox(message)
        }
    }
}

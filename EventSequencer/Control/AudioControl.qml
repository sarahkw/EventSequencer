import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    property Component stripPropertiesComponent: Component {
        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            Item {
                width: 1
                height: 15
            }

            Text {
                text: "File:"
            }
            RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right

                TextField {
                    Layout.fillWidth: true
                    onEditingFinished: {
                        var savtext = text
                        if (cppStrip.audio === null) {
                            cppStrip.initAudio()
                        }
                        cppStrip.audio.fileName = savtext
                    }
                    text: cppStrip.audio !== null ? cppStrip.audio.fileName : ""
                }
                Button {
                    text: "Browse"
                }
            }

        }
    }

}

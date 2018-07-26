import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES

Window {
    width: 640
    height: 480
    title: "Strips List"
    flags: Qt.Dialog

    property bool destroyOnHide: false
    onVisibleChanged: {
        if (destroyOnHide && visible === false) {
            destroy()
        }
    }

    property ES.Document document

    ColumnLayout {
        anchors.fill: parent

        Row {
            TextField {
                id: chan
                placeholderText: "channel"
            }
            TextField {
                id: sf
                placeholderText: "startFrame"
            }
            TextField {
                id: leng
                placeholderText: "length"
            }
            Button {
                text: "Add"
                onClicked: {
                    var r = document.createStrip()
                    r.channel = chan.text
                    r.startFrame = sf.text
                    r.length = leng.text
                    r.markAsPlaced()
                }
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            model: document.stripsModel
            delegate: Row {
                Button {
                    text: "X"
                    onClicked: document.deleteStrip(modelData)
                }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: modelData.channel + ", " + modelData.startFrame + ", " + modelData.length
                }
            }
        }
    }


}

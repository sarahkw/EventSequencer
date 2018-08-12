import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES
import QtQuick.Layouts 1.3

ApplicationWindow {
    id: appwin
    visible: true
    width: 640
    height: 480
    title: qsTr("Document View")
    flags: Qt.Dialog

    property ES.Document document
    property int cursorFrame
    property var changeCursorFrame // Fn

    property bool destroyOnHide: false
    onVisibleChanged: {
        if (destroyOnHide && visible === false) {
            destroy()
        }
    }

    header: RowLayout {
        RowLayout {
            Label {
                text: qsTr("Text Channel")
            }
            ESTextField {
                id: txtTextChannel

                text: dvc.textChannelIndex != null ? dvc.textChannelIndex.toPathString() : ""
                onEsEditingFinished: dvc.textChannelIndex = ES.ChannelIndexFactory.makeFromPathString(text)

                states: [
                    State {
                        when: !dvc.textChannelIsValid
                        PropertyChanges {
                            target: txtTextChannel.background
                            color: "red"
                            ToolTip.visible: txtTextChannel.hovered
                            ToolTip.text: "ERROR: Channel does not provide text for Document View"
                        }
                    }
                ]
            }
        }
        RowLayout {
            Label {
                text: qsTr("Render Channel")
            }
            ESTextField {
                id: txtRenderChannel

                text: dvc.renderChannelIndex != null ? dvc.renderChannelIndex.toPathString() : ""
                onEsEditingFinished: dvc.renderChannelIndex = ES.ChannelIndexFactory.makeFromPathString(text)

                states: [
                    State {
                        when: !dvc.renderChannelIsValid
                        PropertyChanges {
                            target: txtRenderChannel.background
                            color: "red"
                            ToolTip.visible: txtRenderChannel.hovered
                            ToolTip.text: "ERROR: Channel does not render for Document View"
                        }
                    }
                ]
            }
        }
        Item {
            Layout.fillWidth: true
        }
    }

    DocumentViewControl {
        id: dvc
        anchors.fill: parent

        document: appwin.document
        cursorFrame: appwin.cursorFrame
        changeCursorFrame: appwin.changeCursorFrame
    }
}

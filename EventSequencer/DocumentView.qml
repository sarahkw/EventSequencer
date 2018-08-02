import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES
import QtQuick.Layouts 1.3
import "Control/" as Control

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Document View")
    flags: Qt.Dialog

    property ES.Document document

    property bool destroyOnHide: false
    onVisibleChanged: {
        if (destroyOnHide && visible === false) {
            destroy()
        }
    }

    Control.Resolver {
        id: resolver
    }

    ES.ConstrainedMetricsFontUtil {
        id: cmfu
        constrainByWidthValue: 10
    }

    header: RowLayout {
        RowLayout {
            Label {
                text: qsTr("Text Channel")
            }
            ESTextField {
                id: txtTextChannel
                property int chan: 0
                property ES.WaitFor waitFor: document.waitForChannel(chan)
                property var control: waitFor.result != null ? resolver.resolve(waitFor.result.channelType) : null
                property bool providesText: control != null ? control.docViewProvidesText === true : false
                property string textContent: providesText ? waitFor.result.content : ""

                text: chan
                onEsEditingFinished: chan = parseInt(text, 10)

                states: [
                    State {
                        when: !txtTextChannel.providesText
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
                property int chan: 0
                property ES.WaitFor waitFor: document.waitForChannel(chan)
                property var control: waitFor.result != null ? resolver.resolve(waitFor.result.channelType) : null
                property bool canRender: control != null ? control.docViewCanRender === true : false

                text: chan
                onEsEditingFinished: chan = parseInt(text, 10)

                states: [
                    State {
                        when: !txtRenderChannel.canRender
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


    Rectangle {
        anchors.fill: parent
        color: "white"
        ScrollView {
            anchors.fill: parent
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

    ES.WordWrappedTextTrack {
        id: wwtt
        width: lview.width - cmfu.constrainByWidthValue /*For wrapped space or NewLine*/
        text: txtTextChannel.textContent
        font: cmfu.builtFont
    }

}

/*##^## Designer {
    D{i:8;anchors_height:100;anchors_width:100;anchors_x:5;anchors_y:5}
}
 ##^##*/

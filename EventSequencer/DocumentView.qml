import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES
import QtQuick.Layouts 1.3
import "Control/" as Control

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

    Control.Resolver {
        id: resolver
    }

    ES.ConstrainedMetricsFontUtil {
        id: cmfu
        constrainByWidthValue: 8
    }

    header: RowLayout {
        RowLayout {
            Label {
                text: qsTr("Text Channel")
            }
            ESTextField {
                id: txtTextChannel
                
                QtObject {
                    id: chanToTextContent

                    property var input
                    property ES.WaitFor waitFor: input != null ? document.waitForChannelIndex(input) : null
                    property var control: waitFor != null && waitFor.result != null ? resolver.resolve(waitFor.result.channelType) : null
                    property bool providesText: control != null ? control.docViewProvidesText === true : false
                    property var output: providesText ? waitFor.result.content : null
                }

                text: chanToTextContent.input != null ? chanToTextContent.input.toPathString() : ""
                onEsEditingFinished: chanToTextContent.input = ES.ChannelIndexFactory.makeFromPathString(text)

                states: [
                    State {
                        when: chanToTextContent.output === null
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
                property var chan
                property ES.WaitFor waitFor: chan != null ? document.waitForChannelIndex(chan) : null
                property var control: waitFor != null && waitFor.result != null ? resolver.resolve(waitFor.result.channelType) : null
                property var renderComponent: control != null ? control.docViewRenderComponent : null
                property var cppChannel: renderComponent != null ? waitFor.result : null

                text: chan != null ? chan.toPathString() : ""
                onEsEditingFinished: chan = ES.ChannelIndexFactory.makeFromPathString(text)

                states: [
                    State {
                        when: txtRenderChannel.renderComponent == null
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

        ES.WordWrappedTextTrack {
            id: wwtt
            width: lview.width - cmfu.constrainByWidthValue /*For wrapped space or NewLine*/
            text: chanToTextContent.output != null ? chanToTextContent.output : ""
            font: cmfu.builtFont
            cursorFrame: appwin.cursorFrame
        }

        ScrollView {
            id: sview
            anchors.fill: parent
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            clip: true
            ListView {
                id: lview
                model: wwtt
                delegate: Item {
                    height: cmfu.builtFontHeight
                    Item {
                        height: cmfu.builtFontHeight
                        width: modelData.length * cmfu.constrainByWidthValue
                        clip: true
                        Loader {
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            sourceComponent: txtRenderChannel.control != null ? txtRenderChannel.renderComponent : null
                            property var cppChannel: txtRenderChannel.cppChannel
                            property string textData: modelData
                            property int textOffset_: textOffset
                            property int widthPerCharacter: cmfu.constrainByWidthValue
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                appwin.changeCursorFrame(textOffset + Math.floor(mouse.x / cmfu.constrainByWidthValue))
                            }
                        }
                    }
                    Text {
                        text: modelData
                        font: cmfu.builtFont
                    }
                }
            }
            Rectangle {
                id: cursor
                width: 2
                height: cmfu.builtFontHeight
                color: "black"
                property alias cpos: wwtt.cursorPosition
                x: cpos.x * cmfu.constrainByWidthValue
                y: cpos.y * cmfu.builtFontHeight
                z: 10
                readonly property int blinkDelay: 750
                onCposChanged: saVisible.restart()
                SequentialAnimation on visible {
                    id: saVisible
                    loops: Animation.Infinite
                    PropertyAction { value: true }
                    PauseAnimation { duration: cursor.blinkDelay }
                    PropertyAction { value: false }
                    PauseAnimation { duration: cursor.blinkDelay }
                }
            }
        }
    }

}

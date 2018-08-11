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

                    property var inputChannelIndex
                    
                    property ES.WaitFor waitFor: inputChannelIndex != null ? document.waitForChannelIndex(inputChannelIndex) : null
                    property var control: waitFor != null && waitFor.result != null ? resolver.resolve(waitFor.result.channelType) : null
                    property bool providesText: control != null ? control.docViewProvidesText === true : false
                    
                    property var outputTextContent: providesText ? waitFor.result.content : null
                }

                text: chanToTextContent.inputChannelIndex != null ? chanToTextContent.inputChannelIndex.toPathString() : ""
                onEsEditingFinished: chanToTextContent.inputChannelIndex = ES.ChannelIndexFactory.makeFromPathString(text)

                states: [
                    State {
                        when: chanToTextContent.outputTextContent === null
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

                QtObject {
                    id: chanToRenderComponent
                    property var inputChannelIndex
                    
                    property ES.WaitFor waitFor: inputChannelIndex != null ? document.waitForChannelIndex(inputChannelIndex) : null
                    property var control: waitFor != null && waitFor.result != null ? resolver.resolve(waitFor.result.channelType) : null

                    property var outputRenderComponent: control != null ? control.docViewRenderComponent : null
                    property var outputCppChannel: outputRenderComponent != null ? waitFor.result : null
                }

                text: chanToRenderComponent.inputChannelIndex != null ? chanToRenderComponent.inputChannelIndex.toPathString() : ""
                onEsEditingFinished: chanToRenderComponent.inputChannelIndex = ES.ChannelIndexFactory.makeFromPathString(text)

                states: [
                    State {
                        when: chanToRenderComponent.outputRenderComponent == null
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
            text: chanToTextContent.outputTextContent != null ? chanToTextContent.outputTextContent : ""
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
                            sourceComponent: chanToRenderComponent.outputRenderComponent
                            property var cppChannel: chanToRenderComponent.outputCppChannel
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

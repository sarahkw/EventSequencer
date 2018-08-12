import QtQuick 2.9
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES
import "Control/" as Control

Rectangle {
    id: root

    property ES.Document document
    property int cursorFrame
    property var changeCursorFrame // Fn

    property var textChannelIndex
    property bool textChannelIsValid: chanToTextContent.outputTextContent !== null
    property var renderChannelIndex
    property bool renderChannelIsValid: chanToRenderComponent.outputRenderComponent != null
    
    color: "white"

    ES.WordWrappedTextTrack {
        id: wwtt
        width: lview.width - cmfu.constrainByWidthValue /*For wrapped space or NewLine*/
        text: chanToTextContent.outputTextContent != null ? chanToTextContent.outputTextContent : ""
        font: cmfu.builtFont
        cursorFrame: root.cursorFrame
    }

    ES.ConstrainedMetricsFontUtil {
        id: cmfu
        constrainByWidthValue: 8
    }

    Control.Resolver {
        id: resolver
    }

    QtObject {
        id: chanToTextContent
        property ES.WaitFor waitFor: root.textChannelIndex != null ? document.waitForChannelIndex(root.textChannelIndex) : null
        property var control: waitFor != null && waitFor.result != null ? resolver.resolve(waitFor.result.channelType) : null
        property bool providesText: control != null ? control.docViewProvidesText === true : false
        
        property var outputTextContent: providesText ? waitFor.result.content : null
    }

    QtObject {
        id: chanToRenderComponent
        property ES.WaitFor waitFor: root.renderChannelIndex != null ? document.waitForChannelIndex(root.renderChannelIndex) : null
        property var control: waitFor != null && waitFor.result != null ? resolver.resolve(waitFor.result.channelType) : null

        property var outputRenderComponent: control != null ? control.docViewRenderComponent : null
        property var outputCppChannel: outputRenderComponent != null ? waitFor.result : null
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
                            root.changeCursorFrame(textOffset + Math.floor(mouse.x / cmfu.constrainByWidthValue))
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

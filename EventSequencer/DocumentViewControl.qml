import QtQuick 2.9
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES
import "Control/" as Control

Rectangle {
    id: root

    property int cursorFrame
    property var changeCursorFrame // Fn

    property var cppTextChannel
    property var cppRenderChannel

    property bool textChannelIsValid: chanToTextContent.outputTextContent !== null
    property bool renderChannelIsValid: chanToRenderComponent.outputRenderComponent != null
    
    color: "white"

    ES.WordWrappedTextTrack {
        id: wwtt
        width: lview.width - cmfu.builtFontWidth /*For wrapped space or NewLine*/
        text: chanToTextContent.outputTextContent != null ? chanToTextContent.outputTextContent : ""
        font: cmfu.builtFont
        cursorFrame: root.cursorFrame
    }

    ES.ConstrainedMetricsFontUtil {
        id: cmfu
        constrainByWidthEnabled: false
        //baseFont.pointSize: 24
        //constrainByWidthValue: 8
    }

    Control.Resolver {
        id: resolver
    }

    QtObject {
        id: chanToTextContent
        property var control: cppTextChannel !== null ? resolver.resolve(cppTextChannel.channelType) : null
        property bool providesText: control != null ? control.docViewProvidesText === true : false
        
        property var outputTextContent: providesText ? cppTextChannel.content : null
    }

    QtObject {
        id: chanToRenderComponent
        property var control: cppRenderChannel ? resolver.resolve(cppRenderChannel.channelType) : null

        property var outputRenderComponent: control != null ? control.docViewRenderComponent : null
        property var outputCppChannel: outputRenderComponent != null ? cppRenderChannel : null
    }

    function moveCursorLeft() {
        root.changeCursorFrame(Math.max(root.cursorFrame - 1, 0))
    }
    function moveCursorRight() {
        root.changeCursorFrame(Math.min(root.cursorFrame + 1, wwtt.calculateMaxCursorPosition()))
    }
    function moveCursorUp() {
        var cpos = wwtt.cursorPosition
        var newcpos = Qt.point(cpos.x, cpos.y - 1)
        root.changeCursorFrame(wwtt.calculateCursorPositionClosestTo(newcpos))
    }
    function moveCursorDown() {
        var cpos = wwtt.cursorPosition
        var newcpos = Qt.point(cpos.x, cpos.y + 1)
        root.changeCursorFrame(wwtt.calculateCursorPositionClosestTo(newcpos))
    }


    ScrollView {
        id: sview
        anchors.fill: parent
        anchors.leftMargin: 5
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        clip: true

        Keys.onLeftPressed: moveCursorLeft()
        Keys.onRightPressed: moveCursorRight()
        Keys.onUpPressed: moveCursorUp()
        Keys.onDownPressed: moveCursorDown()

        ListView {
            id: lview
            model: wwtt

            delegate: Item {
                height: cmfu.builtFontHeight
                Item {
                    height: cmfu.builtFontHeight
                    width: modelData.length * cmfu.builtFontWidth
                    clip: true
                    Loader {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        sourceComponent: chanToRenderComponent.outputRenderComponent
                        property var cppChannel: chanToRenderComponent.outputCppChannel
                        property string textData: modelData
                        property int textOffset_: textOffset
                        property int widthPerCharacter: cmfu.builtFontWidth
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.changeCursorFrame(textOffset + Math.floor(mouse.x / cmfu.builtFontWidth))
                        }
                    }
                }
                Text {
                    text: modelData
                    textFormat: Text.PlainText
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
            x: cpos.x * cmfu.builtFontWidth
            y: cpos.y * cmfu.builtFontHeight
            z: 10
            readonly property int blinkDelay: 750
            onCposChanged: {
                saVisible.restart()
                makeVisible()
            }
            SequentialAnimation on visible {
                id: saVisible
                loops: Animation.Infinite
                PropertyAction { value: true }
                PauseAnimation { duration: cursor.blinkDelay }
                PropertyAction { value: false }
                PauseAnimation { duration: cursor.blinkDelay }
            }

            function makeVisible() {
                lview.positionViewAtIndex(cpos.y, ListView.Visible)
            }
        }
    }

    // Focus for Keys
    MouseArea {

        // On Android, this prevents scrolling by dragging from working.
        // TODO Figure out why so we don't need this hack.
        //enabled: Qt.platform.os != "android"
        enabled: false // TODO On Desktop, this prevents dragging the scrollbar from working

        anchors.fill: sview
        propagateComposedEvents: true
        onClicked: {
            sview.focus = true
            mouse.accepted = false
        }
    }
}

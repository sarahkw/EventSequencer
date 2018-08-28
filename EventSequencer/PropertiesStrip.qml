import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import "util.js" as Util

import eventsequencer 1.0 as ES

Column {
    spacing: 15

    property bool shouldShowTime: false

    property var document_rebind: document

    GridLayout {
        anchors.left: parent.left
        anchors.right: parent.right

        columns: 2
        Label {
            text: "Channel"
        }
        ESTextField {
            Layout.fillWidth: true
            text: selectedCppStrip.channelIndex.toPathString()
            onEsEditingFinished: {
                var r = ES.ChannelIndexFactory.makeFromPathString(text)
                if (r != null) {
                    selectedCppStrip.channelIndex = r
                } else {
                    // TODO Error handling
                    console.error("Bad channel index")
                }
            }
        }
        Label {
            text: "Start"
        }
        FrameTextField {
            document: document_rebind
            shouldShowTime: shouldShowTime
            frame: selectedCppStrip.startFrame
            onFrameEditingFinished: selectedCppStrip.startFrame = frame
            Layout.fillWidth: true
        }
        Label {
            text: "Length"
        }
        FrameTextField {
            id: unnamedParent_5fa5

            frame: selectedCppStrip.length
            onFrameEditingFinished: selectedCppStrip.length = frame
            Connections { // Reset when c++ rejects
                target: selectedCppStrip
                onLengthChanged: unnamedParent_5fa5.frameChanged()
            }

            document: document_rebind
            shouldShowTime: shouldShowTime
            Layout.fillWidth: true
        }
        Label {
            text: "End"
        }
        FrameTextField {
            // TODO Need to also reset when c++ rejects
            document: document_rebind
            shouldShowTime: shouldShowTime
            frame: selectedCppStrip.startFrame + selectedCppStrip.length
            onFrameEditingFinished: selectedCppStrip.length = frame - selectedCppStrip.startFrame
            Layout.fillWidth: true
        }
    }

    Loader {
        id: stripPropLoader

        anchors.left: parent.left
        anchors.right: parent.right

        readonly property Item blankComponent: Item {}
        property ES.WaitFor waitForchannel: document.waitForChannelIndex(selectedCppStrip.channelIndex)
        property var cppChannel: waitForchannel.result
        property var control: cppChannel !== null ? controlResolver.resolve(cppChannel.channelType) : null
        property var stripPropComp: control !== null ? Util.nvl(control.stripPropertiesComponent, blankComponent) : blankComponent

        sourceComponent: stripPropComp
        property ES.Strip cppStrip: selectedCppStrip
    }

}

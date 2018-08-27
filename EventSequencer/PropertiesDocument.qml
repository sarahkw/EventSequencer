import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQml.Models 2.3

GridLayout {
    id: root

    property bool shouldShowTime: false

    signal msgbox(string message)

    readonly property var document_rebind: document

    columns: 2
    Label {
        text: "Frames/Sec"
    }
    ESTextField {
        Layout.fillWidth: true
        text: document.framesPerSecond
        validator: IntValidator { }
        onEsEditingFinished: document.framesPerSecond = parseInt(text, 10)
    }

    Label {
        text: "Start Frame"
    }
    FrameTextField {
        Layout.fillWidth: true
        document: document_rebind
        shouldShowTime: root.shouldShowTime
        frame: document_rebind.startFrame
        onFrameEditingFinished: document_rebind.startFrame = frame
    }

    Label {
        text: "End Frame"
    }
    FrameTextField {
        Layout.fillWidth: true
        document: document_rebind
        shouldShowTime: root.shouldShowTime
        frame: document_rebind.endFrame
        onFrameEditingFinished: document_rebind.endFrame = frame
    }

    Label {
        text: "Audio Format"
    }
    CheckBox {
        text: "Set"
        checked: document.audioFormatHolderSet
        onCheckedChanged: document.audioFormatHolderSet = checked
        Loader {
            id: propertiesDocumentAudioFormatObjectModelLoader
            sourceComponent: document.audioFormatHolder != null ? comp : null
            property Component comp: Component {
                ObjectModel {
                    Label { text: "\u2514 Sample Rate" }
                    ESTextField {
                        Layout.fillWidth: true
                        text: document.audioFormatHolder.audioFormat.sampleRate
                        onEsEditingFinished: document.audioFormatHolder.audioFormat.sampleRate = parseInt(text, 10)
                    }
                    Label { text: "\u2514 Sample Size" }
                    ESTextField {
                        Layout.fillWidth: true
                        text: document.audioFormatHolder.audioFormat.sampleSize
                        onEsEditingFinished: document.audioFormatHolder.audioFormat.sampleSize = parseInt(text, 10)
                    }
                    Label { text: "\u2514 Channels" }
                    ESTextField {
                        Layout.fillWidth: true
                        text: document.audioFormatHolder.audioFormat.channelCount
                        onEsEditingFinished: document.audioFormatHolder.audioFormat.channelCount = parseInt(text, 10)
                    }
                    Label { text: "\u2514 Sample Type" }
                    ComboBox {
                        Layout.fillWidth: true
                        model: document.audioFormatHolder.sampleTypeModel
                        currentIndex: document.audioFormatHolder.sampleTypeIndex
                        onActivated: document.audioFormatHolder.sampleTypeIndex = index
                    }
                    Label { text: "\u2514 Byte Order" }
                    ComboBox {
                        Layout.fillWidth: true
                        model: document.audioFormatHolder.endianModel
                        currentIndex: document.audioFormatHolder.endianIndex
                        onActivated: document.audioFormatHolder.endianIndex = index
                    }
                    Label { text: "\u2514 Actions" }
                    RowLayout {
                        Layout.fillWidth: true
                        Button {
                            Layout.fillWidth: true
                            text: "Set Default"
                            onClicked: menu.popup()

                            property Menu menu: Menu {
                                MenuItem {
                                    text: "From Input"
                                    onTriggered: {
                                        session.audio.inputPreferredFormat(document.audioFormatHolder)
                                    }
                                }
                                MenuItem {
                                    text: "From Output"
                                    onTriggered: {
                                        session.audio.outputPreferredFormat(document.audioFormatHolder)
                                    }
                                }
                            }
                        }
                        Button {
                            Layout.fillWidth: true
                            text: "Test"

                            onClicked: {
                                root.msgbox(session.audio.testFormatSupport(document.audioFormatHolder))
                            }
                        }
                    }
                }
            }
        }
    }
    Repeater {
        model: propertiesDocumentAudioFormatObjectModelLoader.item
    }

}

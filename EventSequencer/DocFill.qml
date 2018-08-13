import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import eventsequencer 1.0 as ES

ApplicationWindow {
    id: root
    visible: true
    width: 600
    height: 800
    title: qsTr("DocFill")
    flags: Qt.Dialog

    property var cppChannel
    Connections {
        target: cppChannel
        onBeforeDelete: {
            // If cppChannel goes away, delete ourselves immediately so that
            // we don't resolve bindings on "cppChannel" which will be null.
            // Calling "destroy" deletes not soon enough.
            //
            // Note: this assumes that DocFill was created dynamically.
            immediateDestructor.add(root)
        }
    }

    property ES.Session session
    property ES.Document document
    property int cursorFrame
    property var changeCursorFrame // Fn

    property bool destroyOnHide: false
    onVisibleChanged: {
        if (destroyOnHide && visible === false) {
            destroy()
        }
    }

    MessageDialog {
        id: msgbox
        function msgbox(msg) {
            text = msg
            open()
        }
    }

    ES.ErrorReportingContext {
        id: errorReportingContext
    }

    ES.RecorderControl {
        id: recorderControl
        audioFormatHolder: document.audioFormatHolder
        sessionAudio: session.audio
        fileResourceDirectory: document.fileResourceDirectory
        allowOverwrite: true
        onFileDone: corraledResourceFile.done()

        function beginRecord() {
            var begin = corraledResourceFile.begin()
            if (begin.success) {
                record(begin.url)
            } else {
                msgbox.msgbox(begin.errorMsg)
            }
        }

        property CorraledResourceFile corraledResourceFile: CorraledResourceFile {
            fileResourceDirectory: recorderControl.fileResourceDirectory
            corralFileBase: "DOCFILL-UNASSIGNED"
            corralFileSuffix: ".au"
        }

        property ES.ConditionalError unnamed1: ES.ConditionalError {
            errorReportingContext: errorReportingContext
            active: !recorderControl.audioInputReady
            errorText: "Recorder not ready: %1".arg(recorderControl.audioInputReadyStatus)
        }
        property ES.ConditionalError unnamed2: ES.ConditionalError {
            errorReportingContext: errorReportingContext
            active: !(recorderControl.audioState + 0 === ES.RecorderControl.Active ||
                      recorderControl.audioState + 0 === ES.RecorderControl.Stopped)
            errorText: "Recorder in bad state: %1".arg(recorderControl.audioState)
        }
        property ES.ConditionalError unnamed3: ES.ConditionalError {
            errorReportingContext: errorReportingContext
            active: recorderControl.error !== ""
            errorText: "Recorder error: %1".arg(recorderControl.error)
        }
    }

    header: TabBar {
        id: tbar
        currentIndex: sview.currentIndex

        TabButton { text: "Seek" }
        TabButton { text: "Record" }
        TabButton { text: "Selection" }
        TabButton { text: "Properties" }
        TabButton { text: "EVSEQ" }
        TabButton { text: "File" }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Repeater {
            model: errorReportingContext.model
            ErrorMessageNagDelegate {
                Layout.fillWidth: true
            }
        }

        DocumentViewControl {
            Layout.fillWidth: true
            Layout.fillHeight: true

            id: dvc
            document: root.document
            cursorFrame: root.cursorFrame
            changeCursorFrame: root.changeCursorFrame

            textChannelIndex: cppChannel.textChannel
            renderChannelIndex: cppChannel.resourceChannel

            ES.ConditionalError {
                errorReportingContext: errorReportingContext
                active: !dvc.textChannelIsValid
                errorText: "Invalid DocFill Text Channel"
            }

            ES.ConditionalError {
                errorReportingContext: errorReportingContext
                active: !dvc.renderChannelIsValid
                errorText: "Invalid DocFill Resource Channel"
            }
        }
    }

    footer: Frame {
        anchors.left: parent.left
        anchors.right: parent.right
        SwipeView {
            id: sview
            currentIndex: tbar.currentIndex
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            // Take implicitHeight from a normal item
            implicitHeight: recordButtons.implicitHeight

            RowLayout {}

            RowLayout {
                id: recordButtons
                Button {
                    Layout.fillWidth: true
                    text: "Delete"
                    enabled: recorderControl.corraledResourceFile.takeable
                    onClicked: recorderControl.corraledResourceFile.cancel()
                }
                Button {
                    Layout.fillWidth: true
                    text: "Assign"
                }
                Button {
                    Layout.fillWidth: true
                    text: "Preview"
                }
                Button {
                    Layout.fillWidth: true
                    id: unnamedParent_7c13
                    text: "Stop"
                    onClicked: recorderControl.stop()

                    states: [
                        State {
                            when: [
                                ES.RecorderControl.Stopped,
                                ES.RecorderControl.Unconfigured // Want to show the "Record" button
                            ].indexOf(recorderControl.audioState + 0) !== -1
                            PropertyChanges {
                                target: unnamedParent_7c13
                                text: "Record"
                                onClicked: recorderControl.beginRecord()
                            }
                        }
                    ]
                }
            }

            RowLayout {
                Button {
                    Layout.fillWidth: true
                    text: "Start to Cursor"
                }
                Button {
                    Layout.fillWidth: true
                    text: "End to Cursor"
                }
                Button {
                    Layout.fillWidth: true
                    text: "Delete"
                }
            }
            RowLayout {}
            RowLayout {}
            RowLayout {}
        }
    }
}

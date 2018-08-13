import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import eventsequencer 1.0 as ES

ApplicationWindow {
    id: root
    visible: true
    width: 540
    height: 720
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

    property ES.WaitFor waitForTextChannel: cppChannel.textChannel !== null ? document.waitForChannelIndex(cppChannel.textChannel) : null
    property ES.WaitFor waitForResourceChannel: cppChannel.resourceChannel !== null ? document.waitForChannelIndex(cppChannel.resourceChannel) : null
    property var cppTextChannel: waitForTextChannel !== null ? waitForTextChannel.result : null
    property var cppResourceChannel: waitForResourceChannel !== null ? waitForResourceChannel.result : null

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

            function possiblyAssignUrlToStrip(strip) {
                var result = take()
                if (result.success) {
                    strip.resourceUrl = result.newUrl
                    return true
                } else {
                    msgbox.msgbox(result.errorMsg)
                    return false
                }
            }
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
            cursorFrame: root.cursorFrame
            changeCursorFrame: root.changeCursorFrame

            cppTextChannel: root.cppTextChannel
            cppRenderChannel: root.cppResourceChannel

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
                    id: btnRangeStart
                    property int startFrame

                    Layout.fillWidth: true
                    text: "Set Range Start"
                    checkable: true
                    onCheckedChanged: {
                        if (checked) {
                            startFrame = cursorFrame
                        }
                    }
                }
                Button {
                    Layout.fillWidth: true
                    text: "Assign to Range"
                    enabled: cppResourceChannel !== null &&
                             recorderControl.corraledResourceFile.takeable &&
                             btnRangeStart.checked
                    onClicked: {
                        var thestrip =
                                cppResourceChannel.createStrip(btnRangeStart.startFrame,
                                                               cursorFrame - btnRangeStart.startFrame)
                        if (thestrip !== null) {
                            var success = recorderControl.corraledResourceFile.possiblyAssignUrlToStrip(thestrip)
                            if (!success) {
                                // TODO Delete file
                            }

                            btnRangeStart.checked = false
                        } else {
                            msgbox.msgbox("Cannot assign")
                        }
                    }
                }
                /*
                Button {
                    Layout.fillWidth: true
                    text: "Preview"
                }
                */
                Button {
                    Layout.fillWidth: true
                    id: unnamedParent_7c13
                    text: "Stop"
                    onClicked: recorderControl.stop()

                    MessageDialog {
                        id: msgConfirmDelete
                        // Confirm because it's easy to press a button too many
                        // times.
                        text: "Delete unassigned recording?"
                        standardButtons: StandardButton.Yes | StandardButton.No
                        onYes: recorderControl.corraledResourceFile.cancel()
                    }

                    states: [
                        State {
                            when: recorderControl.corraledResourceFile.takeable
                            PropertyChanges {
                                target: unnamedParent_7c13
                                text: "Cancel"
                                onClicked: msgConfirmDelete.open()
                            }
                        },
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

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import "Control/" as C

import eventsequencer 1.0 as ES

Page {
    id: root

    /* Incoming

    // Pass to DocFillPage Begin
    property var cppChannel
    property ES.Session session
    property ES.Document document
    property int cursorFrame
    property var changeCursorFrame // Fn
    // Pass to DocFillPage End

    function closeFn() {
        stackView.pop()
    }
    */

    property int rebind_cursorFrame: cursorFrame
    property var rebind_changeCursorFrame: changeCursorFrame

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

    ES.PlayerControl {
        id: playerControl
        audioFormatHolder: document.audioFormatHolder
        sessionAudio: session.audio
        fileResourceDirectory: document.fileResourceDirectory
        autoStopOnIdle: true

        selectionMode: cmbSelectionMode.currentSelectionMode()
        selectedChannel: cppResourceChannel
        cursorFrame: rebind_cursorFrame
        singleUrl: recorderControl.corraledResourceFile.corralUrl

        property ES.ConditionalError unnamed1: ES.ConditionalError {
            errorReportingContext: errorReportingContext
            active: !playerControl.audioOutputReady
            errorText: "Player not ready: %1".arg(playerControl.audioOutputReadyStatus)
        }
        property ES.ConditionalError unnamed2: ES.ConditionalError {
            errorReportingContext: errorReportingContext
            active: !(playerControl.audioState + 0 === ES.PlayerControl.Active ||
                      playerControl.audioState + 0 === ES.PlayerControl.Stopped)
            errorText: "Player in bad state: %1".arg(playerControl.audioState)
        }
        property ES.ConditionalError unnamed3: ES.ConditionalError {
            errorReportingContext: errorReportingContext
            active: playerControl.error !== ""
            errorText: "Player error: %1".arg(playerControl.error)
        }
    }

    header: TabBar {
        id: tbar
        currentIndex: sview.currentIndex

        TabButton { text: "View" }
        TabButton { text: "Record" }
        TabButton { text: "Play" }
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
            cursorFrame: rebind_cursorFrame
            changeCursorFrame: rebind_changeCursorFrame

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

            // View
            RowLayout {
                Button {
                    Layout.fillWidth: true
                    text: "View"
                    onClicked: viewMenu.open()
                    Menu {
                        id: viewMenu
                        MenuItem {
                            text: "Font..."
                        }
                        Menu {
                            title: "Show"
                            MenuItem {
                                text: "Line"
                            }
                            MenuItem {
                                text: "Position"
                            }
                        }
                    }
                }
                Button {
                    Layout.fillWidth: true
                    text: "Cursor Move"
                    onClicked: goToMenu.open()
                    Menu {
                        id: goToMenu
                        MenuItem {
                            text: "Line..."
                        }
                        MenuItem {
                            text: "Position..."
                        }
                        Menu {
                            title: "Direction"
                            MenuItem { text: "Up"    ; onTriggered: dvc.moveCursorUp() }
                            MenuItem { text: "Down"  ; onTriggered: dvc.moveCursorDown() }
                            MenuItem { text: "Left"  ; onTriggered: dvc.moveCursorLeft() }
                            MenuItem { text: "Right" ; onTriggered: dvc.moveCursorRight() }
                        }
                        MenuItem {
                            text: "Previous Unassigned"
                            onTriggered: {
                                var npos = root.cppResourceChannel.calculateNextEmptyBackward(rebind_cursorFrame)
                                root.rebind_changeCursorFrame(npos)
                            }
                        }
                        MenuItem {
                            text: "Next Unassigned"
                            onTriggered: {
                                var npos = root.cppResourceChannel.calculateNextEmptyForward(rebind_cursorFrame)
                                root.rebind_changeCursorFrame(npos)
                            }
                        }
                        MenuItem {
                            text: "Previous Segment"
                        }
                        MenuItem {
                            text: "Next Segment"
                        }
                    }
                }
            }

            // Record
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
                                text: "Delete"
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

            // Play
            RowLayout {
                ComboBox {
                    id: cmbSelectionMode
                    Layout.fillWidth: true
                    model: [
                        "Recording",
                        "At Cursor",
                        "From Cursor",
                        "Tone"
                    ]
                    function currentSelectionMode() {
                        switch (currentIndex) {
                        case -1:
                        case 0: return ES.PlayerControl.SingleUrl
                        case 1: return ES.PlayerControl.ChannelOnCursor
                        case 2: return ES.PlayerControl.ChannelFromCursor
                        case 3: return ES.PlayerControl.SingleUrl // XXX Not implemented
                        default:
                            console.error("Invalid index", currentIndex)
                        }
                    }
                }
                Button {
                    text: "Show"
                }
                Button {
                    id: unnamedParent_6bbd
                    //Layout.fillWidth: true
                    text: "Stop"
                    onClicked: playerControl.stop()
                    states: [
                        State {
                            when: ([
                                ES.PlayerControl.Stopped,
                                ES.PlayerControl.Unconfigured // Want to show the "Record" button
                            ].indexOf(playerControl.audioState + 0) !== -1 && playerControl.error !== "")

                            PropertyChanges {
                                target: unnamedParent_6bbd
                                text: "Clear Error"
                                onClicked: playerControl.stop()
                            }
                        },
                        State {
                            when: [
                                ES.PlayerControl.Stopped,
                                ES.PlayerControl.Unconfigured // Want to show the "Record" button
                            ].indexOf(playerControl.audioState + 0) !== -1

                            PropertyChanges {
                                target: unnamedParent_6bbd
                                text: "Play"
                                onClicked: playerControl.play()
                            }
                        }
                    ]
                }
            }
            // File
            RowLayout {
                id: fileFooterItem
                Button {
                    Layout.fillWidth: true
                    text: "Save"
                    onClicked: {
                        if (document.currentUrl === "") {
                            console.error("Don't have a current URL to save to?")
                        } else {
                            document.save(document.currentUrl)
                        }
                    }
                }
                Button {
                    Layout.fillWidth: true
                    text: "Close"
                    onClicked: {
                        fileDrawer.visible = false // If I don't do this it looks weird
                        closeFn()
                    }
                }
            }
        }
    }

    Drawer {
        id: fileDrawer
        parent: root.contentItem
        height: root.contentItem.height
        width: 0.95 * root.contentItem.width
        edge: Qt.RightEdge

        interactive: false
        visible: fileFooterItem.SwipeView.isCurrentItem

        Page {
            anchors.fill: parent
            clip: true
            header: TabBar {
                id: fileDrawerTabBar
                currentIndex: fileDrawerSwipeView.currentIndex
                TabButton { text: "Strips" }
                TabButton { text: "Properties" }
                TabButton { text: "Resources" }
                TabButton { text: "Export" }
            }
            SwipeView {
                id: fileDrawerSwipeView
                anchors.fill: parent
                currentIndex: fileDrawerTabBar.currentIndex

                Item { }

                ScrollView {
                    contentWidth: width // Column will have some width it wants to be. Ignore it.
                    Column {
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.topMargin: 15
                        anchors.leftMargin: 15
                        anchors.rightMargin: 15
                        spacing: 15

                        GroupBox {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            title: "DocFill"
                            Loader {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                C.DocFillControl {
                                    id: docFillControl
                                }
                                sourceComponent: docFillControl.docFillPropertiesComponent
                            }
                        }

                        GroupBox {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            title: "Application"
                            GridLayout {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                columns: 2

                                CheckBox {
                                    Layout.fillWidth: true
                                    Layout.columnSpan: 2
                                    text: "Automatically start range on record"
                                }
                                CheckBox {
                                    Layout.fillWidth: true
                                    Layout.columnSpan: 2
                                    text: "Automatically play after record"
                                }
                                
                                CheckBox {
                                    Layout.fillWidth: true
                                    text: "Pitch utilities"
                                }
                                Button {
                                    text: "Configure"
                                }
                            }
                        }

                        GroupBox {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            title: "Raw Properties"
                            ComboBox {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                model: [
                                    "",
                                    "Selected Strip via Strips View",
                                    "Selected Strip via Text View",
                                    "Docfill Channel",
                                    "Text Channel",
                                    "Specific Channel",
                                    "Document",
                                    "File",
                                    "Session",
                                ]
                            }
                        }
                    }
                }

                Item { }

                Item { }
            }
        }
    }
}

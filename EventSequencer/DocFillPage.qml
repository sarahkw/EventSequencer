import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import "Control/" as C

// Don't know why but "Dialog" is the native one otherwise.
import QtQuick.Controls 2.2 as Controls2

import eventsequencer 1.0 as ES
import eventsequencer.playable 1.0 as ESPlayable

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

    AutoSaveManager {
        id: autoSaveManager
        function saveFn() {
            return documentSaveOrShowError(document.currentUrl)
        }
        autoSaveEnabled: chkAutoSave.checked
    }

    MsgBox  {
        id: msgbox
    }

    function documentSaveOrShowError(url) {
        if (url === "") {
            msgbox.msgbox("No URL to save to", "Save failure")
            return false
        }
        
        var result = document.save(url)
        var success = result[0]
        if (success) {
            return true
        } else {
            var errorMsg = result[1]
            msgbox.msgbox(errorMsg, "Save failure")
            return false
        }
    }

    ES.ErrorReportingContext {
        id: errorReportingContext
    }

    // XXX The CorraledResourceFile also has a copy of this. Maybe we should
    //     pass it a ref.
    ES.ManagedResources {
        id: managedResources
        fileResourceDirectory: document.fileResourceDirectory
    }

    ES.RecorderControl {
        id: recorderControl
        audioFormatHolder: document.audioFormatHolder
        sessionAudio: session.audio
        fileResourceDirectory: document.fileResourceDirectory
        allowOverwrite: true
        onFileDone: {
            corraledResourceFile.done()
            if (chkSettingReviewAfterRecord.checked) {
                playPage.reviewAfterRecord()
            }
        }

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
                    msgbox.msgbox(result.errorMsg, "Cannot assign resource to strip")
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

    ESPlayable.SingleUrl {
        id: playableSingleUrl
        fileResourceDirectory: document.fileResourceDirectory
        singleUrl: recorderControl.corraledResourceFile.corralUrl
    }

    ESPlayable.StripsList {
        id: playableStripsList
        fileResourceDirectory: document.fileResourceDirectory
        selectionMode: cmbSelectionMode.currentSelectionMode()
        selectedChannel: cppResourceChannel
        cursorFrame: rebind_cursorFrame

        onPlayingFrame: {
            dvc.positionMode = ListView.Center
            rebind_changeCursorFrame(frameNumber)
            dvc.resetPositionMode()
        }
    }

    ESPlayable.Tone {
        id: playableTone
        //frequency: 220
    }

    ES.PlayerControl {
        id: playerControl
        audioFormatHolder: document.audioFormatHolder
        sessionAudio: session.audio
        autoStopOnIdle: true

        playable: cmbSelectionMode.currentPlayable()

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
        TabButton {
            text: recordPage.isStopped ? "Record" : "Recording"
        }
        TabButton {
            text: playPage.isStopped ? "Play" : "Playing"
        }
        TabButton {
            text: autoSaveManager.isDirty ? "File*" : "File"
        }
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

    // This is a Component because it is used in multiple
    // places. Calling .open() from different buttons doesn't
    // work. Calling .popup() works but makes the menu show up
    // differently than calling .open().
    Component {
        id: goToMenuComponent
        Menu {
            MenuItem {
                text: "Scroll to Cursor"
                onTriggered: {
                    dvc.showCursor()
                }
            }
            MenuItem {
                text: "Position..."
                Controls2.Dialog {
                    id: positionDialog

                    parent: ApplicationWindow.overlay
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2

                    title: "Change Cursor Position"
                    modal: true
                    standardButtons: Controls2.Dialog.Ok | Controls2.Dialog.Cancel
                    TextField {
                        id: positionDialogTextField
                        anchors.centerIn: parent
                        implicitWidth: 100
                        inputMethodHints: Qt.ImhDigitsOnly
                        text: cursorFrame
                    }
                    onAccepted: changeCursorFrame(positionDialogTextField.text)
                }
                onTriggered: {
                    positionDialog.open()
                }
            }
            MenuItem {
                text: "Previous Interesting"
                onTriggered: {
                    var npos = root.cppResourceChannel.calculateNextSegmentBackward(rebind_cursorFrame)
                    root.rebind_changeCursorFrame(npos)
                }
            }
            MenuItem {
                text: "Next Interesting"
                onTriggered: {
                    var npos = root.cppResourceChannel.calculateNextSegmentForward(rebind_cursorFrame)
                    root.rebind_changeCursorFrame(npos)
                }
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
            implicitHeight: recordPage.implicitHeight

            // View
            RowLayout {
                Button {
                    Layout.fillWidth: true
                    text: "View"
                    onClicked: viewMenu.open()
                    Menu {
                        id: viewMenu
                        MenuItem {
                            text: "Copy All"
                            onTriggered: ES.ClipboardWrapper.setText(cppTextChannel.content)
                        }
                        MenuItem {
                            text: "Font"
                            onTriggered: fontDialog.open()
                        }
                    }
                    Controls2.Dialog {
                        id: fontDialog

                        parent: ApplicationWindow.overlay
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2

                        title: "Change Font Size"
                        modal: true
                        standardButtons: Controls2.Dialog.Ok
                        SpinBox {
                            id: fontDialogTextField
                            anchors.centerIn: parent
                            from: 1
                            to: 72
                            editable: true
                            value: dvc.font.pointSize
                            onValueModified: {
                                dvc.font.pointSize = value
                                focus = false
                            }
                        }
                    }
                }

                Button {
                    Layout.fillWidth: true
                    text: "Go"
                    onClicked: goToMenu.item.open()
                    Loader {
                        id: goToMenu
                        sourceComponent: goToMenuComponent
                    }
                }
            }

            // Record
            RowLayout {
                id: recordPage

                property bool isStopped: [
                    ES.RecorderControl.Stopped,
                    ES.RecorderControl.Unconfigured
                ].indexOf(recorderControl.audioState + 0) !== -1
                
                Button {
                    id: btnRangeStart
                    property int startFrame

                    Layout.fillWidth: true
                    text: "Select"
                    checkable: true
                    onCheckedChanged: {
                        if (checked) {
                            startFrame = cursorFrame
                        }
                    }
                    states: [
                        State {
                            when: btnRangeStart.checked
                            PropertyChanges {
                                target: btnRangeStart
                                text: "Select (%1)".arg(cursorFrame - startFrame)
                            }
                        }
                    ]
                }
                Button {
                    Layout.fillWidth: true
                    text: "Assign"
                    enabled: cppResourceChannel !== null &&
                             recorderControl.corraledResourceFile.takeable &&
                             btnRangeStart.checked
                    onClicked: {

                        var start
                        var length
                        if (btnRangeStart.startFrame < cursorFrame) {
                            start = btnRangeStart.startFrame
                            length = cursorFrame - start
                        } else {
                            start = cursorFrame
                            length = btnRangeStart.startFrame - start
                        }

                        var thestrip =
                                cppResourceChannel.createStrip(start, length)
                        if (thestrip !== null) {
                            var success = recorderControl.corraledResourceFile.possiblyAssignUrlToStrip(thestrip)
                            if (!success) {
                                // An error has already been shown.
                                document.deleteStrip(thestrip)
                            } else {
                                btnRangeStart.checked = false
                                autoSaveManager.markDirty()
                            }
                        } else {
                            // TODO Explain why, like it overlaps, or the length is bad
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

                    Timer {
                        id: tmrPlayToneBeforeRecord1
                        interval: 2000
                        onTriggered: {
                            playPage.stopToneBeforeRecord()
                            tmrPlayToneBeforeRecord2.start()
                        }
                    }

                    Timer {
                        // XXX If I start recording right after the tone stops,
                        // some of the tone gets recorded. Maybe this other timer
                        // is a race condition? I guess we'll have to find out
                        // a nice way to wait for the tone to stop. Maybe we have
                        // to wait out the audio buffer size?
                        id: tmrPlayToneBeforeRecord2
                        interval: 100
                        onTriggered: {
                            recorderControl.beginRecord()
                        }
                    }

                    states: [
                        State {
                            when: recorderControl.corraledResourceFile.takeable
                            PropertyChanges {
                                target: unnamedParent_7c13
                                text: "Delete Unassigned"
                                onClicked: msgConfirmDelete.open()
                            }
                        },
                        State {
                            when: tmrPlayToneBeforeRecord1.running || tmrPlayToneBeforeRecord2.running
                            PropertyChanges {
                                target: unnamedParent_7c13
                                text: "Cancel Tone"
                                onClicked: {
                                    tmrPlayToneBeforeRecord1.stop()
                                    tmrPlayToneBeforeRecord2.stop()
                                    playPage.stopToneBeforeRecord()
                                }
                            }
                        },
                        State {
                            when: recordPage.isStopped
                            PropertyChanges {
                                target: unnamedParent_7c13
                                text: "Record"
                                onClicked: {
                                    if (chkSettingSelectOnRecord.checked) {
                                        btnRangeStart.checked = false
                                        btnRangeStart.checked = true
                                    }
                                    if (chkSettingToneBeforeRecord.checked) {
                                        playPage.playToneBeforeRecord()
                                        tmrPlayToneBeforeRecord1.start()
                                    } else {
                                        recorderControl.beginRecord()
                                    }
                                }
                            }
                        }
                    ]
                }
            }

            // Play
            RowLayout {
                id: playPage

                property bool isStopped: [
                    ES.PlayerControl.Stopped,
                    ES.PlayerControl.Unconfigured
                ].indexOf(playerControl.audioState + 0) !== -1

                function reviewAfterRecord() {
                    playerControl.stop()
                    cmbSelectionMode.currentIndex = 0
                    playerControl.play()
                }

                function playToneBeforeRecord() {
                    playerControl.stop()
                    cmbSelectionMode.currentIndex = 4
                    playerControl.play()
                }

                function stopToneBeforeRecord() {
                    playerControl.stop()
                }

                ComboBox {
                    id: cmbSelectionMode
                    Layout.fillWidth: true
                    model: [
                        "Review Unassigned Recording",
                        "At Cursor",
                        "From Cursor",
                        "From Start",
                        "Tone"
                    ]
                    function currentPlayable() {
                        switch (currentIndex) {
                        case -1:
                        case 0: return playableSingleUrl
                        case 1: return playableStripsList
                        case 2: return playableStripsList
                        case 3: return playableStripsList
                        case 4: return playableTone
                        default:
                            console.error("Invalid index", currentIndex)
                        }
                    }
                    function currentSelectionMode() {
                        switch (currentIndex) {
                        case -1:
                        case 0: return ESPlayable.StripsList.UNSET
                        case 1: return ESPlayable.StripsList.ChannelOnCursor
                        case 2: return ESPlayable.StripsList.ChannelFromCursor
                        case 3: return ESPlayable.StripsList.ChannelFromBegin
                        case 4: return ESPlayable.StripsList.UNSET
                        default:
                            console.error("Invalid index", currentIndex)
                        }
                    }
                }
                TextField {
                    visible: cmbSelectionMode.currentIndex === 4
                    placeholderText: "Hz"
                    text: playableTone.frequency !== 0 ? playableTone.frequency : null
                    implicitWidth: 80
                    inputMethodHints: Qt.ImhDigitsOnly
                    onEditingFinished: {
                        playableTone.frequency = parseInt(text, 10)
                        focus = false
                    }
                }
                Button {
                    id: unnamedParent_6bbd
                    //Layout.fillWidth: true
                    text: "Stop"
                    onClicked: playerControl.stop()
                    states: [
                        State {
                            when: (playPage.isStopped && playerControl.error !== "")

                            PropertyChanges {
                                target: unnamedParent_6bbd
                                text: "Clear Error"
                                onClicked: playerControl.stop()
                            }
                        },
                        State {
                            when: playPage.isStopped

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
                    id: btnCloseFile
                    text: "Close File"

                    function actuallyClose() {
                        fileDrawer.visible = false // If I don't do this it looks weird
                        closeFn()
                    }

                    MessageDialog {
                        id: closeFileDirtyConfirm
                        title: "Unsaved changes"
                        standardButtons: StandardButton.Yes | StandardButton.No | StandardButton.Cancel
                        text: "Would you like to save the document before closing?"
                        onYes: {
                            if (documentSaveOrShowError(document.currentUrl)) {
                                btnCloseFile.actuallyClose()
                            }
                        }
                        onNo: btnCloseFile.actuallyClose()
                    }

                    onClicked: {
                        if (autoSaveManager.isDirty) {
                            closeFileDirtyConfirm.open()
                        } else {
                            btnCloseFile.actuallyClose()
                        }
                    }
                }
                Button {
                    id: chkAutoSave
                    Layout.fillWidth: true
                    text: "Auto Save"
                    checkable: true
                    checked: true
                }
                Button {
                    Layout.fillWidth: true
                    text: "Save File"
                    onClicked: {
                        documentSaveOrShowError(document.currentUrl)
                        autoSaveManager.markManualSaved()
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
                TabButton { text: "Storage" }
                TabButton { text: "Export" }
                TabButton { text: "Settings" }
            }
            SwipeView {
                id: fileDrawerSwipeView
                anchors.fill: parent
                currentIndex: fileDrawerTabBar.currentIndex

                // Strips
                Item {
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 15
                        Frame {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Rectangle {
                                id: stripsBody
                                anchors.fill: parent
                                color: "white"
                                clip: true

                                property int spaceToTakeUp: parent.width
                                property var cmfuAlignedFont: ES.ConstrainedMetricsFontUtil {
                                    constrainByWidthEnabled: false
                                }
                                property int charactersToShow: spaceToTakeUp / cmfuAlignedFont.builtFontWidth + 2 /*Rounding, I guess. Can't hurt*/
                                property int charactersShifted: charactersToShow / 2

                                Loader {
                                    id: textLoader
                                    anchors.left: cursorObj.left

                                    property alias charactersToShow: stripsBody.charactersToShow
                                    property alias charactersShifted: stripsBody.charactersShifted
                                    property alias cmfuAlignedFont: stripsBody.cmfuAlignedFont

                                    C.TextControl {
                                        id: textControl
                                        property var cppChannel: root.cppTextChannel
                                    }
                                    sourceComponent: textControl.docFillStripsTrackComponent
                                }

                                Rectangle {
                                    id: dragArea
                                    color: "whitesmoke"
                                    radius: 5
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.bottom: parent.bottom
                                    height: cursorObj.draggableSectionHeight

                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.left: parent.left
                                        anchors.leftMargin: 10
                                        color: "lightgrey"
                                        text: "<"
                                        font.pixelSize: parent.height / 2
                                    }
                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.right: parent.right
                                        anchors.rightMargin: 10
                                        color: "lightgrey"
                                        text: ">"
                                        font.pixelSize: parent.height / 2
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        property real initialX: 0
                                        preventStealing: true // don't swipe to next page
                                        onPressed: initialX = mouseX
                                        onPositionChanged: {
                                            var xdiff = mouse.x - initialX
                                            var fontWidth = stripsBody.cmfuAlignedFont.builtFontWidth
                                            var fullFrames
                                            if (xdiff > 0) {
                                                fullFrames = Math.floor(xdiff / fontWidth)
                                                if (fullFrames > 0) {
                                                    initialX += fullFrames * fontWidth
                                                    root.rebind_changeCursorFrame(rebind_cursorFrame - fullFrames)
                                                }
                                            } else if (xdiff < 0) {
                                                fullFrames = -Math.ceil(xdiff / fontWidth)
                                                if (fullFrames > 0) {
                                                    initialX -= fullFrames * fontWidth
                                                    root.rebind_changeCursorFrame(rebind_cursorFrame + fullFrames)
                                                }
                                            }
                                        }
                                    }
                                }

                                Cursor {
                                    id: cursorObj
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: cursorFrame
                                    width: 1
                                }

                                Item {
                                    anchors.left: cursorObj.left
                                    anchors.top: textLoader.bottom
                                    anchors.topMargin: 5
                                    ES.WatchForStripsIntersectingRange {
                                        id: wfsir
                                        channel: root.cppResourceChannel.sourceChannel
                                        startFrame: cursorFrame - stripsBody.charactersShifted
                                        length: stripsBody.charactersToShow
                                    }
                                    Item {
                                        id: stripsHolderItem

                                        property QtObject selectedStrip: null
                                        Connections {
                                            target: wfsir
                                            onStripsChanged: {
                                                if (stripsHolderItem.selectedStrip !== null &&
                                                        !wfsir.isStripInView(stripsHolderItem.selectedStrip)) {
                                                    stripsHolderItem.selectedStrip = null
                                                }
                                            }
                                        }

                                        Repeater {
                                            model: wfsir.strips
                                            Button {
                                                x: (modelData.startFrame - cursorFrame) * stripsBody.cmfuAlignedFont.builtFontWidth
                                                width: modelData.length * stripsBody.cmfuAlignedFont.builtFontWidth
                                                y: (height + 5) * modelData.channelIndex.second
                                                checkable: true
                                                height: stripsBody.cmfuAlignedFont.builtFontHeight * 1.5
                                                palette.button: "pink"
                                                Component.onCompleted: {
                                                    background.border.width = 1
                                                    background.border.color = "black"
                                                }
                                                checked: stripsHolderItem.selectedStrip === modelData
                                                onToggled: {
                                                    if (checked) {
                                                        stripsHolderItem.selectedStrip = modelData
                                                    } else {
                                                        stripsHolderItem.selectedStrip = null
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Button {
                                Layout.fillWidth: true
                                text: "Delete..."
                                enabled: stripsHolderItem.selectedStrip !== null
                                onClicked: {
                                    deleteStripDialog.open()
                                }

                                Controls2.Dialog {
                                    id: deleteStripDialog
                                    parent: ApplicationWindow.overlay
                                    x: (parent.width - width) / 2
                                    y: (parent.height - height) / 2
                                    width: parent.width * 0.8

                                    title: "Delete Strip"
                                    modal: true

                                    onOpened: deleteStripOption1.checked = true

                                    footer: DialogButtonBox {
                                        Button {
                                            text: "Delete Strip"
                                            onClicked: {
                                                var thestrip = stripsHolderItem.selectedStrip

                                                function deleteTheStrip() {
                                                    document.deleteStrip(thestrip)
                                                    autoSaveManager.markDirty()
                                                }

                                                if (thestrip.resourceUrl == "") {
                                                    deleteTheStrip()
                                                    return
                                                }

                                                if (deleteStripOption1.checked) {
                                                    var success = recorderControl.corraledResourceFile.recorral(thestrip.resourceUrl)
                                                    if (success) {
                                                        deleteTheStrip()
                                                    } else {
                                                        msgbox.msgbox("Unable to reclaim recording. Is there already an unassigned recording?")
                                                    }
                                                } else if (deleteStripOption2.checked) {
                                                    var success = managedResources.deleteUrl(thestrip.resourceUrl)
                                                    if (success) {
                                                        deleteTheStrip()
                                                    } else {
                                                        msgbox.msgbox("Unable to delete recording")
                                                    }
                                                } else if (deleteStripOption3.checked) {
                                                    deleteTheStrip()
                                                }

                                                deleteStripDialog.close()
                                            }
                                        }
                                        Button {
                                            text: "Cancel"
                                            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                                        }
                                    }

                                    ColumnLayout {
                                        Label {
                                            Layout.fillWidth: true
                                            text: "Action to take on strip's recording:"
                                            wrapMode: Text.Wrap
                                        }
                                        RadioButton {
                                            Layout.fillWidth: true
                                            id: deleteStripOption1
                                            text: "Reclaim as unassigned recording"
                                        }
                                        RadioButton {
                                            Layout.fillWidth: true
                                            id: deleteStripOption2
                                            text: "Delete recording"
                                        }
                                        RadioButton {
                                            Layout.fillWidth: true
                                            id: deleteStripOption3
                                            text: "Leave recording as orphan"
                                        }
                                    }
                                }
                            }
                            Button {
                                id: goButton
                                Layout.fillWidth: true
                                text: "Go"
                                onClicked: goToMenu2.item.open()
                                Loader {
                                    id: goToMenu2
                                    sourceComponent: goToMenuComponent
                                }
                            }
                        }
                    }
                }

                // Storage
                Item {
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 15
                        ScrollView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            contentWidth: width
                            clip: true
                            Column {
                                anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.right: parent.right
                                spacing: 15
                                GroupBox {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    title: "Information"
                                    GridLayout {
                                        columns: 2
                                        Label { text: "Report Age" }
                                        Label {
                                            Layout.fillWidth: true
                                            text: "2 minutes ago"
                                        }
                                        
                                        Label { text: "Active Resource Size" }
                                        Label {
                                            Layout.fillWidth: true
                                            text: "80 MB"
                                        }

                                        Label { text: "Orphan Resource Size" }
                                        Label {
                                            Layout.fillWidth: true
                                            text: "10 MB"
                                        }
                                    }
                                }
                                GroupBox {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    title: "Orphaned Resources"
                                    RowLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        Button {
                                            Layout.fillWidth: true
                                            text: "Copy List"
                                        }
                                        Button {
                                            Layout.fillWidth: true
                                            text: "Delete All"
                                        }
                                    }
                                }

                                GroupBox {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    title: "Strips Missing Resources"
                                    RowLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        Button {
                                            Layout.fillWidth: true
                                            text: "Copy List"
                                        }
                                        Button {
                                            Layout.fillWidth: true
                                            text: "Delete All"
                                        }
                                    }
                                }
                            }
                        }
                        Button {
                            Layout.fillWidth: true
                            text: "Generate Report"
                        }
                    }
                }

                // Export
                ScrollView {
                    contentWidth: width
                    Column {
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.topMargin: 15
                        anchors.leftMargin: 15
                        anchors.rightMargin: 15
                        spacing: 15

                        DocFillPageExportSection {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            title: "Export JSON"
                            description: "Write a JSON file that lists the parts of the document along with the assigned audio file."
                        }

                        DocFillPageExportSection {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            title: "Play to File"
                            description: "Play as configured in the 'Play' tab, but output to a file instead of to your speakers."
                        }
                    }
                }

                // Settings
                Item {
                    ScrollView {
                        anchors.fill: parent
                        anchors.bottomMargin: 15 // TODO HACK: I don't know why this is necessary else bottom gets cut.
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
                                title: "Application"
                                GridLayout {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    columns: 2

                                    CheckBox {
                                        id: chkSettingReviewAfterRecord
                                        Layout.fillWidth: true
                                        Layout.columnSpan: 2
                                        text: "Automatically play review after record"
                                    }
                                    CheckBox {
                                        id: chkSettingSelectOnRecord
                                        Layout.fillWidth: true
                                        Layout.columnSpan: 2
                                        text: "Automatically start selection on record"
                                    }
                                    CheckBox {
                                        id: chkSettingToneBeforeRecord
                                        Layout.fillWidth: true
                                        Layout.columnSpan: 2
                                        text: "Play tone before record"
                                    }
                                    CheckBox {
                                        id: chkSettingDeveloperOptions
                                        Layout.fillWidth: true
                                        Layout.columnSpan: 2
                                        text: "Enable developer options"
                                    }
                                }
                            }

                            Loader {
                                id: devOptionsLoader
                                anchors.left: parent.left
                                anchors.right: parent.right
                                source: chkSettingDeveloperOptions.checked ? "DocFillDeveloperOptions.qml" : ""
                                property alias selectedCppStrip: stripsHolderItem.selectedStrip
                            }
                            Connections {
                                target: devOptionsLoader.item
                                onMsgbox: msgbox.msgbox(message)
                            }
                        }
                    }
                }
            }
        }
    }
}

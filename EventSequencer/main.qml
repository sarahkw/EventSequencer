import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0 as Qlp
import "util.js" as Util
import "Control" as Ctrl

import eventsequencer 1.0 as ES

ApplicationWindow {
    id: appwin
    visible: true
    width: 800
    height: 600
    title: "EventSequencer%1".arg(document.currentFileName != "" ? " [%1]".arg(document.currentFileName) : "")

    menuBar: MenuBar {
        CustomMenu {
            title: "File"
            Action {
                text: "New"
                onTriggered: document.reset()
                shortcut: "Ctrl+N"
            }
            Action {
                id: openAction
                text: "Open"
                onTriggered: openFileDialog.open()
                shortcut: "Ctrl+O"
            }
            Action {
                id: saveAction
                text: "Save"
                onTriggered: {
                    if (document.currentUrl == "") {
                        saveFileDialog.open()
                    } else {
                        document.save(document.currentUrl)
                    }
                }
                shortcut: "Ctrl+S"
            }
            Action {
                text: "Save As"
                onTriggered: saveFileDialog.open()
            }
            Action {
                text: "Revert"
                onTriggered: document.load(document.currentUrl)
                enabled: document.currentUrl !== ""
            }
            Action {
                text: "Quit"
                onTriggered: Qt.quit()
            }
        }
        CustomMenu {
            title: "View"
            Action {
                text: "Go to Origin"
                onTriggered: {
                    body.x = 0
                    body.y = 0
                }
            }
            Action {
                text: "Go to Cursor"
                onTriggered: {
                    var p = body.mapFromItem(cursor, 0, 0).x
                    body.x = -p
                }
                shortcut: "Alt+Home"
            }
            MenuSeparator { }
            Action {
                id: showSecondsAction
                text: "Show Seconds"
                checkable: true
                shortcut: "Ctrl+T"
            }
            MenuSeparator { }
            Action {
                id: propertiesAction
                text: "Properties"
                checkable: true
                shortcut: "N"
            }
        }
        CustomMenu {
            title: "Select"
            Action {
                text: "(De)select All"
                shortcut: "A"
                onTriggered: {
                    if (selectedCppStrips.length != 0) {
                        selectedCppStrips = []
                    } else {
                        // Select All
                        var strips = document.stripsForQml()
                        strips.forEach(function (s) {
                            // This sucks, but user could have had the left/right selected.
                            s.qmlStrip.selectionMode = SelectionMode.Whole
                        })
                        selectedCppStrips = strips
                    }
                }
            }
        }
        CustomMenu {
            id: addMenu
            title: "Add"
            MenuItem {
                text: "Add (Shift+A)"
                enabled: false
            }
            MenuSeparator { }
            Action {
                text: "&Strip"
                onTriggered: {
                    var cppStrip = document.createStrip()
                    cppStrip.channel = channelPanel.activeChannel
                    cppStrip.startFrame = cursor.frame
                    cppStrip.length = 10 // Maybe make this one large tick instead
                    cppStrip.markAsPlaced()
                    selectedCppStrips = [cppStrip]
                }
            }
        }
        CustomMenu {
            title: "Strip"
            Action {
                id: grabAction
                text: "Grab"
                checkable: true
                shortcut: "G"
            }
            Action {
                text: "Delete"
                onTriggered: {
                    selectedCppStrips.forEach(function (foo) {
                        document.deleteStrip(foo)
                    })
                    selectedCppStrips = []
                }
                shortcut: "X"
            }
        }
        CustomMenu {
            title: "Tools"
            Action {
                text: "Strips List"
                onTriggered: {
                    var component = Qt.createComponent("StripsList.qml")
                    component.createObject(appwin, {
                                               visible: true,
                                               document: document,
                                               destroyOnHide: true
                                           })
                }
            }
            Action {
                text: "Resource Creator"
                onTriggered: {
                    var component = Qt.createComponent("ResourceCreator.qml")
                    component.createObject(appwin, {
                                               visible: true,
                                               destroyOnHide: true
                                           })
                }
            }
        }
        CustomMenu {
            title: "Debug"
            CustomMenuItem {
                text: "ConstrainedMetricsFontUtil"
                onTriggered: {
                    var component = Qt.createComponent("DebugConstrainedMetricsFontUtil.qml")
                    var obj = component.createObject(appwin, {visible: true})
                }
            }
            CustomMenuItem {
                text: "WordWrappedTextTrack"
                onTriggered: {
                    var component = Qt.createComponent("DebugWordWrappedTextTrack.qml")
                    var obj = component.createObject(appwin, {visible: true})
                }
            }
            CustomMenuItem {
                text: "DebugChannelsModel"
                onTriggered: {
                    var component = Qt.createComponent("DebugChannelsModel.qml")
                    var obj = component.createObject(appwin, {visible: true, model: document.channelsModel})
                }
            }
            CustomMenuItem {
                text: "JS Garbage Collect"
                onTriggered: gc()
            }
            CustomMenuItem {
                text: "Dump Protobuf"
                onTriggered: {
                    document.dumpProtobuf()
                }
            }
        }

    }

    Action {
        id: addAction
        text: "Add"
        onTriggered: addMenu.open()
        shortcut: "Shift+A"
    }

    ES.Document {
        id: document
    }

    Ctrl.Resolver {
        id: controlResolver
    }

    Item {
        property alias clockChannel: cmbClockChannel.clockChannel
        property ES.WaitFor waitForChannel: clockChannel !== null ? document.waitForChannel(clockChannel) : null
        property var channel: waitForChannel !== null ? waitForChannel.result : null
        property var control: channel !== null ? controlResolver.resolve(channel.channelType) : null
        property var clockComponent: control !== null ? control.clockComponent : null

        Loader {
            sourceComponent: parent.clockComponent

            property ES.Document document: document
            property int currentFrame: cursor.frame
            property bool isPlaying: playButton.checked
            property bool isLoop: playButton.loop
            function playFrame(frameNo) {
                cursor.frame = frameNo

                document.stripsOnFrame(frameNo).forEach(function (cppStrip) {
                    var chan = cppStrip.channel;
                    var waitFor = document.waitForChannel(chan);
                    if (waitFor.result === null) {
                        return;
                    }
                    var chanType = waitFor.result.channelType;
                    var ctrl = controlResolver.resolve(chanType);
                    if ("playFrame" in ctrl) {
                        ctrl.playFrame(cppStrip, frameNo - cppStrip.startFrame);
                    }
                });
            }
        }
    }

    footer: ToolBar {
        Row {
            Label {
                text: "("
                anchors.verticalCenter: parent.verticalCenter
            }
            FrameTextField {
                anchors.verticalCenter: parent.verticalCenter
                document: document
                shouldShowTime: showSecondsAction.checked
                frame: document.startFrame
                onFrameEditingFinished: document.startFrame = frame
                horizontalAlignment: TextInput.AlignRight
                ToolTip.text: "Start"
                ToolTip.visible: hovered
            }
            Label {
                text: "-"
                anchors.verticalCenter: parent.verticalCenter
            }
            FrameTextField {
                anchors.verticalCenter: parent.verticalCenter
                document: document
                shouldShowTime: showSecondsAction.checked
                frame: document.endFrame
                onFrameEditingFinished: document.endFrame = frame
                horizontalAlignment: TextInput.AlignRight
                ToolTip.text: "End"
                ToolTip.visible: hovered
            }
            Label {
                text: ") @"
                anchors.verticalCenter: parent.verticalCenter
            }

            FrameTextField {
                anchors.verticalCenter: parent.verticalCenter
                document: document
                shouldShowTime: showSecondsAction.checked
                frame: cursor.frame
                onFrameEditingFinished: cursor.frame = frame
                horizontalAlignment: TextInput.AlignRight
                ToolTip.text: "Current"
                ToolTip.visible: hovered
            }
            QtObject {
                id: controlUnicode
                readonly property string play: "\u25B6" // Black Right-Pointing Triangle
                readonly property string stop: "\u23F9" // Black Square for Stop
                readonly property string prev: "\u23EE" // Black Left-Pointing Double Triangle With Vertical Bar
                readonly property string next: "\u23ED" // Black Right-Pointing Double Triangle With Vertical Bar
            }

            Label {
                text: "   "
                anchors.verticalCenter: parent.verticalCenter
            }
            RoundButton {
                anchors.verticalCenter: parent.verticalCenter
                text: controlUnicode.prev
                onClicked: cursor.frame = document.startFrame
            }
            RoundButton {
                ToolTip.text: "Play/Stop (long press for menu)"
                ToolTip.visible: hovered

                property bool loop: false
                id: playButton
                anchors.verticalCenter: parent.verticalCenter
                text: checked ? controlUnicode.stop : controlUnicode.play
                checkable: true
                Menu {
                    id: playMenu
                    MenuItem {
                        text: "Loop Play"
                        onClicked: {
                            playButton.loop = true
                            playButton.checked = true
                        }
                    }
                }
                onPressAndHold: {
                    playMenu.open()
                }
                onClicked: playButton.loop = false
            }
            RoundButton {
                anchors.verticalCenter: parent.verticalCenter
                text: controlUnicode.next
                onClicked: cursor.frame = document.endFrame
            }
            Label {
                text: "   "
                anchors.verticalCenter: parent.verticalCenter
            }
            ComboBox {
                property var clockChannel: null

                property var cache: document.channelsProvidingClock
                property bool blockSignal: false

                onCacheChanged: {
                    var newIndex = clockChannel === null ? -1 : cache.indexOf(clockChannel)

                    if (newIndex === -1 && cache.length > 0) {
                        newIndex = 0
                    }

                    blockSignal = true
                    model = cache
                    currentIndex = newIndex
                    blockSignal = false

                    var newClockChannel = currentIndex === -1 ? null : cache[currentIndex]

                    if (clockChannel !== newClockChannel) {
                        clockChannel = newClockChannel
                    }
                }

                id: cmbClockChannel
                ToolTip.text: "Clock channel"
                ToolTip.visible: hovered
                anchors.verticalCenter: parent.verticalCenter
                width: 50
                model: cache

                onCurrentIndexChanged: {
                    if (!blockSignal) {
                        clockChannel = cache[currentIndex]
                    }
                }
            }

            Label {
                id: statusBar
                text: "Status"
                elide: Text.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
                anchors.verticalCenter: parent.verticalCenter
                leftPadding: 10
            }
        }
    }

    property var displayFrameNumber: (function (shouldShowTime, framesPerSecond) {
        return function (frameNumber, hideFramesWhenHoursShown) {
            if (shouldShowTime)
                return ES.FramesAndSeconds.framesToSeconds(
                    framesPerSecond, frameNumber, hideFramesWhenHoursShown);
            else
                return frameNumber;
        }
    })(showSecondsAction.checked, document.framesPerSecond)

    Qlp.FileDialog {
        id: saveFileDialog
        fileMode: Qlp.FileDialog.SaveFile
        nameFilters: ["Event sequencer files (*.evseq)", "All files (*)"]
        defaultSuffix: "evseq"

        onAccepted: {
            document.save(file)
        }
    }
    Qlp.FileDialog {
        id: openFileDialog
        fileMode: Qlp.FileDialog.OpenFile
        nameFilters: ["Event sequencer files (*.evseq)", "All files (*)"]
        onAccepted: document.load(file)
    }

    // "needle in haystack" doesn't seem to work for QML elements
    function realIn(needle, haystack) {
        return haystack.some(function (foo) { return foo === needle });
    }

    property var selectedCppStrips: []

    property ZoomLogic zoom: ZoomLogic {}
    property int channelPixels: 30

    Item {
        anchors.fill: parent
        Item {
            id: twoDView
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: propertiesDragger.left

            ChannelPanel {
                id: channelPanel
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: channelDragger.left
                channelPixels: appwin.channelPixels
                yposition: body.y
                doc: document
            }

            Dragger {
                id: channelDragger
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                position: 150
                color: "lightgrey"
            }

            MouseArea {
                anchors.fill: bodyView
                acceptedButtons: Qt.RightButton
                function xyPositionToChannel(x, y) {
                    var whereWas = body.mapFromItem(bodyView, x, y).y
                    var channelAtMouse = Math.floor(whereWas / channelPixels)
                    return channelAtMouse
                }
                onClicked: {
                    selectedCppStrips = []
                    channelPanel.activeChannel = xyPositionToChannel(mouse.x, mouse.y)
                }
            }

            Rectangle {
                id: bodyView
                anchors.left: channelDragger.right
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: sbHoriz.top
                clip: true // Strips will definitely go out of bounds

                color: "#fafafa"

                Rectangle {
                    // White background for positive x/y
                    anchors.top: body.top
                    anchors.left: body.left
                    anchors.right: bodyView.right
                    anchors.bottom: bodyView.bottom
                    color: "white"
                }

                Rectangle {
                    property int frame: 0
                    id: cursor
                    width: 2
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    color: "lime"
                    x: body.x + zoom.mapFrameToDisplayX(frame) - 1
                    z: 1

                    Rectangle {
                        color: cursor.color
                        anchors.left: cursor.right
                        anchors.top: cursorText.top
                        anchors.bottom: cursorText.bottom
                        anchors.right: cursorText.right
                        anchors.rightMargin: -10
                        anchors.topMargin: -2
                        anchors.bottomMargin: -2
                    }

                    Text {
                        id: cursorText
                        anchors.left: cursor.right
                        anchors.bottom: cursor.bottom
                        anchors.leftMargin: 10
                        anchors.bottomMargin: 10
                        text: displayFrameNumber(cursor.frame)
                        font.pixelSize: 12
                    }
                }

                Item {
                    // This moves around inside bodyView.
                    id: body
                    implicitWidth: childrenRect.width + childrenRect.x
                    implicitHeight: childrenRect.height + childrenRect.y

                    property ES.ConstrainedMetricsFontUtil cmfuAlignedFont: ES.ConstrainedMetricsFontUtil {
                        constrainByWidthValue: zoom.displayWidthPerRulerTick
                        addLetterSpacingToMatchWidth: true
                        constrainByHeightEnabled: true
                        constrainByHeightValue: channelPixels
                    }

                    Repeater {
                        model: document.channelsModel
                        Loader {
                            sourceComponent: controlResolver.resolve(modelData.channelType).channelTrackComponent
                            property var cppChannel: modelData
                            y: channelIndex * channelPixels
                            height: channelPixels
                            property ES.ConstrainedMetricsFontUtil cmfuAlignedFont: body.cmfuAlignedFont
                            property ZoomLogic zoom: appwin.zoom
                        }
                    }

                    Repeater {
                        model: document.stripsModel
                        Item {
                            id: stripBase

                            property int selectionMode: SelectionMode.Whole
                            property bool selected: realIn(cppStrip, selectedCppStrips)

                            function _selectionClicked(mouse, newSelectionMode) {
                                if (mouse.modifiers & Qt.ShiftModifier) {
                                    var tmp = selectedCppStrips
                                    selectedCppStrips = []

                                    if (realIn(cppStrip, tmp)) {
                                        tmp = tmp.filter(function (foo) {
                                            return foo !== cppStrip;
                                        })
                                    } else {
                                        tmp.push(cppStrip)
                                    }

                                    selectedCppStrips = tmp
                                } else {
                                    selectedCppStrips = [cppStrip]
                                }

                                stripBase.selectionMode = newSelectionMode

                                channelPanel.activeChannel = cppStrip.channel
                            }

                            Loader {
                                sourceComponent: stripComponentResolver.stripComponent === undefined ? dsvComponent : undefined
                                anchors.fill: parent

                                Component {
                                    id: dsvComponent
                                    Item {
                                        anchors.fill: parent
                                        DefaultStripView {
                                            id: dsv
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            anchors.verticalCenter: parent.verticalCenter

                                            selected: stripBase.selected
                                            selectionMode: stripBase.selectionMode
                                            onSelectionClicked: _selectionClicked(mouse, newSelectionMode)
                                        }
                                    }
                                }
                            }

                            QtObject {
                                id: stripComponentResolver
                                property ES.WaitFor waitFor: document.waitForChannel(cppStrip.channel)
                                property var chan: waitFor.result
                                property var type: chan !== null ? chan.channelType : null
                                property var stripComponent: type !== null ? controlResolver.resolve(type).stripComponent : undefined
                            }

                            Loader {
                                sourceComponent: stripComponentResolver.stripComponent
                                anchors.fill: parent

                                property ES.Strip cppStrip: stripBase.cppStrip
                                property bool selected: stripBase.selected
                                property int selectionMode: stripBase.selectionMode

                                property ES.ConstrainedMetricsFontUtil cmfuAlignedFont: body.cmfuAlignedFont

                                function selectionClicked() {
                                    _selectionClicked.apply(this, arguments)
                                }
                            }

                            readonly property ES.Strip cppStrip: modelData

                            Component.onCompleted: {
                                cppStrip.qmlStrip = stripBase
                            }

                            x: zoom.mapFrameToDisplayX(cppStrip.startFrame)
                            width: Math.max(
                                       Math.max(
                                           zoom.mapLengthToDisplayWidth(cppStrip.length),
                                           zoom.displayWidthPerRulerTick / 2),
                                       3 // Hard limit.
                                       )
                            y: cppStrip.channel * channelPixels
                            height: channelPixels

                            function floorDiv(a, b) {
                                return Math.floor(a / b);
                            }

                            Connections {
                                target: grabMode
                                onFinalCommit: {
                                    // Not using diffX and diffY passed in because
                                    // it doesn't work in debug mode.
                                    // https://bugreports.qt.io/browse/QTBUG-68522
                                    var myDiffX = grabMode.diffX
                                    var myDiffY = grabMode.diffY

                                    if (selected) {
                                        switch (stripBase.selectionMode) {
                                        case SelectionMode.Whole:
                                            cppStrip.startFrame += zoom.mapDisplayWidthToFullFrames(myDiffX)
                                            cppStrip.channel += floorDiv(myDiffY, channelPixels)
                                            break;
                                        case SelectionMode.Right:
                                            cppStrip.length += zoom.mapDisplayWidthToFullFrames(myDiffX)
                                            break;
                                        case SelectionMode.Left:
                                            var initialEndFrame = cppStrip.startFrame + cppStrip.length
                                            cppStrip.length -= zoom.mapDisplayWidthToFullFrames(myDiffX)
                                            cppStrip.startFrame = initialEndFrame - cppStrip.length
                                            break;
                                        }
                                    }
                                }
                            }

                            property int initialFrame
                            property int initialChannel
                            property int initialLength

                            StateGroup {
                                states: [
                                    State {
                                        name: "move_whole"
                                        when: (selected &&
                                               grabMode.grabState == grabMode.grabstate_MOVING &&
                                               stripBase.selectionMode === SelectionMode.Whole)
                                        PropertyChanges {
                                            target: stripBase
                                            explicit: true
                                            initialFrame: cppStrip.startFrame
                                            initialChannel: cppStrip.channel
                                        }

                                        PropertyChanges {
                                            target: cppStrip
                                            startFrame: (initialFrame +
                                                         zoom.mapDisplayWidthToFullFrames(grabMode.diffX))
                                            channel: (initialChannel +
                                                      floorDiv(grabMode.diffY, channelPixels))
                                        }
                                    },
                                    State {
                                        name: "move_right"
                                        when: (selected &&
                                               grabMode.grabState == grabMode.grabstate_MOVING &&
                                               stripBase.selectionMode === SelectionMode.Right)
                                        PropertyChanges {
                                            target: stripBase
                                            explicit: true
                                            initialLength: cppStrip.length
                                        }

                                        PropertyChanges {
                                            target: cppStrip
                                            length: (initialLength +
                                                     zoom.mapDisplayWidthToFullFrames(grabMode.diffX))
                                        }
                                    },
                                    State {
                                        name: "move_left"
                                        when: (selected &&
                                               grabMode.grabState == grabMode.grabstate_MOVING &&
                                               stripBase.selectionMode === SelectionMode.Left)
                                        PropertyChanges {
                                            target: stripBase
                                            explicit: true
                                            initialFrame: cppStrip.startFrame + cppStrip.length
                                            initialLength: cppStrip.length
                                        }

                                        PropertyChanges {
                                            target: cppStrip
                                            length: (initialLength -
                                                     zoom.mapDisplayWidthToFullFrames(grabMode.diffX))
                                            startFrame: initialFrame - cppStrip.length
                                        }
                                    }
                                ]
                            }
                        }
                    }
                }
            }

            Ruler {
                id: sbHoriz
                height: 20
                zoom: appwin.zoom
                xposition: body.x
                displayFrameNumber: appwin.displayFrameNumber

                anchors.left: bodyView.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }

            MouseArea {
                id: pan
                anchors.fill: bodyView
                acceptedButtons: Qt.MiddleButton

                property int lastMouseX
                property int lastMouseY

                property int anglePerPixel: 120
                property int angleAccumulator: 0

                onPressed: {
                    lastMouseX = mouse.x
                    lastMouseY = mouse.y
                }
                onPositionChanged: {
                    var deltaX = mouse.x - lastMouseX
                    var deltaY = mouse.y - lastMouseY
                    var newx = body.x + deltaX
                    var newy = body.y + deltaY
                    body.x = newx
                    body.y = newy
                    lastMouseX = mouse.x
                    lastMouseY = mouse.y
                }

                onWheel: {
                    angleAccumulator += wheel.angleDelta.y
                    var whole = Math.floor(angleAccumulator / anglePerPixel)
                    var remain = angleAccumulator % anglePerPixel
                    angleAccumulator = remain

                    var whereWasTheFrame = body.mapFromItem(bodyView, wheel.x, wheel.y).x
                    var frameNoAtMouse = zoom.mapDisplayWidthToFrames(whereWasTheFrame)
                    zoom.zoom(whole)

                    // Move body.x so that the old frame number stays in the same place.
                    var whereIsTheFrameNow = zoom.mapFrameToDisplayX(frameNoAtMouse)
                    body.x += whereWasTheFrame - whereIsTheFrameNow
                }
            }

            MouseArea {
                id: cursorMouseArea
                anchors.fill: bodyView
                function xyPositionToFrame(x, y) {
                    var whereWas = body.mapFromItem(bodyView, x, y).x
                    var frameNoAtMouse = zoom.mapDisplayWidthToFrames(whereWas)
                    return frameNoAtMouse
                }
                onPressed: {
                    bodyView.focus = true // Otherwise focus will be stuck in edit controls.
                    cursor.frame = xyPositionToFrame(mouse.x, mouse.y)
                }
                onPositionChanged: {
                    cursor.frame = xyPositionToFrame(mouse.x, mouse.y)
                }
            }

            MouseArea {
                id: grabMode
                enabled: grabAction.checked
                hoverEnabled: true
                // Accept all buttons so we don't let mouse events through. Otherwise
                // it will get confusing when we deselect stuff being grabbed, for
                // example.
                acceptedButtons: Qt.AllButtons
                anchors.fill: bodyView

                readonly property int grabstate_INACTIVE: 0
                readonly property int grabstate_PREMOVE1: 1
                readonly property int grabstate_PREMOVE2: 2
                readonly property int grabstate_MOVING: 3

                property int grabState: grabstate_INACTIVE
                property int initialMouseX
                property int initialMouseY
                property int diffX
                property int diffY

                signal finalCommit(int diffX, int diffY)

                onEnabledChanged: {
                    switch (grabState) {
                    case grabstate_INACTIVE:
                        if (enabled) {
                            grabState = grabstate_PREMOVE1
                        }
                        break;
                    case grabstate_PREMOVE1:
                    case grabstate_PREMOVE2:
                        if (!enabled) {
                            grabState = grabstate_INACTIVE
                        }
                        break;
                    case grabstate_MOVING:
                        if (!enabled) {
                            grabState = grabstate_INACTIVE
                            finalCommit(diffX, diffY)
                        }
                        break;
                    }
                }

                onPositionChanged: {
                    switch (grabState) {
                    case grabstate_INACTIVE:
                        break;
                    case grabstate_PREMOVE1:
                        initialMouseX = mouseX
                        initialMouseY = mouseY
                        grabState = grabstate_PREMOVE2
                        break;
                    case grabstate_PREMOVE2:
                    case grabstate_MOVING:
                        diffX = mouseX - initialMouseX
                        diffY = mouseY - initialMouseY
                        grabState = grabstate_MOVING // XXX Only set if not already moving?
                        break;
                    }
                }

                function cancel() {
                    switch (grabState) {
                    case grabstate_INACTIVE:
                    case grabstate_PREMOVE1:
                    case grabstate_PREMOVE2:
                    case grabstate_MOVING:
                        grabState = grabstate_INACTIVE
                        grabAction.checked = false
                        break;
                    }
                }

                onClicked: {
                    if (mouse.button === Qt.LeftButton) {
                        // Confirm
                        grabAction.checked = false
                    } else if (mouse.button === Qt.RightButton) {
                        // Cancel
                        cancel()
                    }
                }
            }
        }

        Dragger {
            id: propertiesDragger
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            position: 200
            flip: true

            states: [
                State {
                    when: !propertiesAction.checked
                    PropertyChanges {
                        target: propertiesDragger
                        position: 0
                        visible: false
                    }
                }
            ]
        }

        Pane {
            id: propertiesPanel
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: propertiesDragger.right

            Component {
                // Blank component so that the Loader resizes itself upon
                // clearing.
                id: blankComponent
                Item {}
            }

            ScrollView {
                anchors.fill: parent
                contentWidth: width // Column will have some width it wants to be. Ignore it.
                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    spacing: 15

                    // strip properties
                    Loader {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        sourceComponent: selectedCppStrips.length == 1 ? stripPropertiesComponent : blankComponent
                        property alias dokument: document

                        Component {
                            id: stripPropertiesComponent
                            Column {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                spacing: 15

                                property ES.Strip selectedCppStrip: selectedCppStrips[0]

                                Label {
                                    text: "Strip"
                                    font.pixelSize: 16
                                    font.bold: true
                                }

                                GridLayout {
                                    anchors.left: parent.left
                                    anchors.right: parent.right

                                    columns: 2
                                    Label {
                                        text: "Channel"
                                    }
                                    ESTextField {
                                        Layout.fillWidth: true
                                        text: selectedCppStrip.channel
                                        validator: IntValidator { }
                                        onEsEditingFinished: selectedCppStrip.channel = parseInt(text, 10)
                                    }
                                    Label {
                                        text: "Start"
                                    }
                                    FrameTextField {
                                        document: dokument
                                        shouldShowTime: showSecondsAction.checked
                                        frame: selectedCppStrip.startFrame
                                        onFrameEditingFinished: selectedCppStrip.startFrame = frame
                                        Layout.fillWidth: true
                                    }
                                    Label {
                                        text: "Length"
                                    }
                                    FrameTextField {
                                        document: dokument
                                        shouldShowTime: showSecondsAction.checked
                                        frame: selectedCppStrip.length
                                        onFrameEditingFinished: selectedCppStrip.length = frame
                                        Layout.fillWidth: true
                                    }
                                    Label {
                                        text: "End"
                                    }
                                    FrameTextField {
                                        document: dokument
                                        shouldShowTime: showSecondsAction.checked
                                        frame: selectedCppStrip.startFrame + selectedCppStrip.length
                                        onFrameEditingFinished: selectedCppStrip.length = frame - selectedCppStrip.startFrame
                                        Layout.fillWidth: true
                                    }
                                }

                                Loader {
                                    id: stripPropLoader

                                    anchors.left: parent.left
                                    anchors.right: parent.right

                                    property ES.WaitFor waitForchannel: document.waitForChannel(selectedCppStrip.channel)
                                    property var channel: waitForchannel.result
                                    property var control: channel !== null ? controlResolver.resolve(channel.channelType) : null
                                    property var stripPropComp: control !== null ? control.stripPropertiesComponent : undefined

                                    sourceComponent: stripPropComp
                                    property ES.Strip cppStrip: selectedCppStrip
                                }
                            }
                        }
                    } // strip properties

                    // channel properties
                    Loader {
                        anchors.left: parent.left
                        anchors.right: parent.right

                        property ES.WaitFor waitForchannel: document.waitForChannel(channelPanel.activeChannel)
                        property var channel: waitForchannel.result
                        property var control: channel !== null ? controlResolver.resolve(channel.channelType) : null
                        property var chanPropComp: (
                                                       (control !== null && control.channelPropertiesComponent !== undefined) ?
                                                           control.channelPropertiesComponent :
                                                           null
                                                       )

                        sourceComponent: ((chanPropComp !== null &&
                                           selectedCppStrips.every(function (cppStrip) {
                                               return cppStrip.channel === channelPanel.activeChannel
                                           })) ? channelPropertiesComponent : blankComponent)

                        Component {
                            id: channelPropertiesComponent
                            Column {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                spacing: 15
                                Label {
                                    text: "Channel"
                                    font.pixelSize: 16
                                    font.bold: true
                                }
                                Loader {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    sourceComponent: chanPropComp

                                    property var cppChannel: channel
                                }
                            }
                        }
                    } // channel properties

                    // document properties
                    Column {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 15

                        Label {
                            text: "Document"
                            font.pixelSize: 16
                            font.bold: true
                        }

                        GridLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right

                            columns: 2
                            Label {
                                text: "Frames/Sec"
                            }
                            ESTextField {
                                Layout.fillWidth: true
                                text: document.framesPerSecond
                                validator: IntValidator { }
                                onEsEditingFinished: {
                                    document.framesPerSecond = parseInt(text, 10)
                                }
                            }
                        }
                    } // document properties

                    // file properties
                    Column {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 15

                        Label {
                            text: "File"
                            font.pixelSize: 16
                            font.bold: true
                        }

                        GridLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right

                            columns: 2
                            Label { text: "Forked From" }
                            ESTextField {
                                Layout.fillWidth: true
                                readOnly: !propertiesForkEdit.allowForkedFromEdit
                                text: document.fileForkedFromChecksum
                                onEsEditingFinished: document.fileForkedFromChecksum = text
                            }
                            Label { text: "Fork Action" }
                            ComboBox {
                                id: propertiesForkEdit
                                Layout.fillWidth: true
                                ToolTip.visible: hovered
                                ToolTip.text: "Action to take during next save"
                                model: ["", "Auto Set", "Clear", "User Edit"]
                                readonly property bool allowForkedFromEdit: currentIndex === 3
                                onActivated: {
                                    switch (index) {
                                    case 0: document.fileForkEditFlag = ES.Document.ForkEditFlag.None; break
                                    case 1: document.fileForkEditFlag = ES.Document.ForkEditFlag.Set; break
                                    case 2: document.fileForkEditFlag = ES.Document.ForkEditFlag.Clear; break
                                    case 3: document.fileForkEditFlag = ES.Document.ForkEditFlag.Custom; break
                                    }
                                }
                                currentIndex: {
                                    switch (document.fileForkEditFlag) {
                                    case ES.Document.ForkEditFlag.None  : return 0
                                    case ES.Document.ForkEditFlag.Set   : return 1
                                    case ES.Document.ForkEditFlag.Clear : return 2
                                    case ES.Document.ForkEditFlag.Custom: return 3
                                    }
                                }
                            }
                            Label { text: "Resource Dir" }
                            ESTextField {
                                Layout.fillWidth: true
                                readOnly: true
                                text: document.fileResourceDirectory
                            }
                        }
                    } // file properties

                }
            } // ScrollView

        }
    }
}

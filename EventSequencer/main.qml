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
    title: qsTr("Hello World")

    menuBar: MenuBar {
        CustomMenu {
            title: "File"
            Action {
                id: openAction
                text: "Open"
                onTriggered: openFileDialog.open()
                shortcut: "Ctrl+O"
            }
            Action {
                id: saveAction
                text: "Save"
                onTriggered: saveFileDialog.open()
                shortcut: "Ctrl+S"
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
                        var strips = document.strips()
                        strips.forEach(function (s) {
                            // This sucks, but user could have had the left/right selected.
                            s.qmlStrip.selectionMode = s.qmlStrip.selectionMode_WHOLE
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
            title: "Debug"
            CustomMenuItem {
                text: "DebugCppModel"
                onTriggered: {
                    var component = Qt.createComponent("DebugCppModel.qml")
                    var obj = component.createObject(appwin, {visible: true})
                }
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
            function changeFrame(newFrame) {
                cursor.frame = newFrame
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
                onFrameEditingFinished: {
                    document.startFrame = frame
                    focus = false
                }
                selectByMouse: true
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
                onFrameEditingFinished: {
                    document.endFrame = frame
                    focus = false
                }
                selectByMouse: true
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
                onFrameEditingFinished: {
                    cursor.frame = frame
                    focus = false
                }

                selectByMouse: true
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
                id: playButton
                anchors.verticalCenter: parent.verticalCenter
                text: checked ? controlUnicode.stop : controlUnicode.play
                checkable: true
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
        nameFilters: ["Data files (*.dat)", "All files (*)"]
        onAccepted: document.save(file)
    }
    Qlp.FileDialog {
        id: openFileDialog
        fileMode: Qlp.FileDialog.OpenFile
        nameFilters: ["Data files (*.dat)", "All files (*)"]
        onAccepted: document.load(file)
    }

    // "needle in haystack" doesn't seem to work for QML elements
    function realIn(needle, haystack) {
        return haystack.some(function (foo) { return foo === needle });
    }

    property var selectedCppStrips: []

    property ZoomLogic zoom: ZoomLogic {}
    property int channelPixels: 35

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
                onClicked: selectedCppStrips = []
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

                    Repeater {
                        model: document
                        Strip {
                            id: strip

                            readonly property ES.Strip cppStrip: modelData

                            Component.onCompleted: {
                                cppStrip.qmlStrip = strip
                            }

                            x: zoom.mapFrameToDisplayX(cppStrip.startFrame)
                            width: Math.max(zoom.mapLengthToDisplayWidth(cppStrip.length), minimumWidth)
                            y: cppStrip.channel * channelPixels

                            selected: realIn(cppStrip, selectedCppStrips)

                            onSelectionClicked: {
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
                            }

                            function floorDiv(a, b) {
                                return Math.floor(a / b);
                            }

                            Connections {
                                target: grabMode
                                onFinalCommit: {
                                    if (selected) {
                                        switch (strip.selectionMode) {
                                        case strip.selectionMode_WHOLE:
                                            cppStrip.startFrame += zoom.mapDisplayWidthToFullFrames(diffX)
                                            cppStrip.channel += floorDiv(diffY, channelPixels)
                                            break;
                                        case strip.selectionMode_RIGHT:
                                            cppStrip.length += zoom.mapDisplayWidthToFullFrames(diffX)
                                            break;
                                        case strip.selectionMode_LEFT:
                                            var initialEndFrame = cppStrip.startFrame + cppStrip.length
                                            cppStrip.length -= zoom.mapDisplayWidthToFullFrames(diffX)
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
                                               strip.selectionMode == strip.selectionMode_WHOLE)
                                        PropertyChanges {
                                            target: strip
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
                                               strip.selectionMode == strip.selectionMode_RIGHT)
                                        PropertyChanges {
                                            target: strip
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
                                               strip.selectionMode == strip.selectionMode_LEFT)
                                        PropertyChanges {
                                            target: strip
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
                    var whereWasTheFrame = body.mapFromItem(bodyView, x, y).x
                    var frameNoAtMouse = zoom.mapDisplayWidthToFrames(whereWasTheFrame)
                    return frameNoAtMouse
                }

                onPressed: {
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
            anchors.leftMargin: 5
            anchors.rightMargin: 5

            ColumnLayout {
                anchors.fill: parent

                // strip properties
                Loader {
                    Layout.fillWidth: true
                    sourceComponent: selectedCppStrips.length == 1 ? stripPropertiesComponent : blankComponent
                    property alias dokument: document

                    Component {
                        // Blank component so that the Loader resizes itself upon
                        // clearing.
                        id: blankComponent
                        Item {}
                    }

                    Component {
                        id: stripPropertiesComponent
                        Column {
                            anchors.left: parent.left
                            anchors.right: parent.right

                            property ES.Strip selectedCppStrip: selectedCppStrips[0]

                            Label {
                                text: "Strip"
                                font.pixelSize: 16
                                font.bold: true
                            }

                            Item { // Spacer
                                width: 1
                                height: 15
                            }

                            GridLayout {
                                anchors.left: parent.left
                                anchors.right: parent.right

                                columns: 2
                                Label {
                                    text: "Channel"
                                }
                                TextField {
                                    Layout.fillWidth: true
                                    text: selectedCppStrip.channel
                                    selectByMouse: true
                                    validator: IntValidator { }
                                    onEditingFinished: {
                                        selectedCppStrip.channel = parseInt(text, 10)
                                        focus = false
                                    }
                                }
                                Label {
                                    text: "Start"
                                }
                                FrameTextField {
                                    document: dokument
                                    shouldShowTime: showSecondsAction.checked
                                    frame: selectedCppStrip.startFrame
                                    onFrameEditingFinished: {
                                        selectedCppStrip.startFrame = frame
                                        focus = false
                                    }

                                    Layout.fillWidth: true
                                    selectByMouse: true
                                }
                                Label {
                                    text: "Length"
                                }
                                FrameTextField {
                                    document: dokument
                                    shouldShowTime: showSecondsAction.checked
                                    frame: selectedCppStrip.length
                                    onFrameEditingFinished: {
                                        selectedCppStrip.length = frame
                                        focus = false
                                    }

                                    Layout.fillWidth: true
                                    selectByMouse: true
                                }
                                Label {
                                    text: "End"
                                }
                                FrameTextField {
                                    document: dokument
                                    shouldShowTime: showSecondsAction.checked
                                    frame: selectedCppStrip.startFrame + selectedCppStrip.length
                                    onFrameEditingFinished: {
                                        selectedCppStrip.length = frame - selectedCppStrip.startFrame
                                        focus = false
                                    }

                                    Layout.fillWidth: true
                                    selectByMouse: true
                                }
                            }

                            Loader {
                                anchors.left: parent.left
                                anchors.right: parent.right

                                property ES.WaitFor waitForchannel: document.waitForChannel(selectedCppStrip.channel)
                                property var channel: waitForchannel.result
                                property var control: channel !== null ? controlResolver.resolve(channel.channelType) : null
                                property var stripPropComp: control !== null ? control.stripPropertiesComponent : null

                                sourceComponent: stripPropComp
                                property ES.Strip cppStrip: selectedCppStrip
                            }

                            Item { // End Spacer
                                width: 1
                                height: 15
                            }

                        }
                    }
                } // strip properties

                // channel properties
                Loader {
                    Layout.fillWidth: true

                    property ES.WaitFor waitForchannel: document.waitForChannel(channelPanel.activeChannel)
                    property var channel: waitForchannel.result
                    property var control: channel !== null ? controlResolver.resolve(channel.channelType) : null
                    property var chanPropComp: control !== null ? control.channelPropertiesComponent : null

                    sourceComponent: chanPropComp !== null ? channelPropertiesComponent : blankComponent

                    Component {
                        id: channelPropertiesComponent
                        Column {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            Label {
                                text: "Channel"
                                font.pixelSize: 16
                                font.bold: true
                            }
                            Item { // Spacer
                                width: 1
                                height: 15
                            }
                            Loader {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                sourceComponent: chanPropComp
                            }
                            Item { // End Spacer
                                width: 1
                                height: 15
                            }
                        }
                    }
                } // channel properties

                // document properties
                Column {
                    Layout.fillWidth: true

                    Label {
                        text: "Document"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Item { // Spacer
                        width: 1
                        height: 15
                    }

                    GridLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right

                        columns: 2
                        Label {
                            text: "Frames/Sec"
                        }
                        TextField {
                            Layout.fillWidth: true
                            text: document.framesPerSecond
                            selectByMouse: true
                            validator: IntValidator { }
                            onEditingFinished: {
                                document.framesPerSecond = parseInt(text, 10)
                                focus = false
                            }
                        }
                    }
                } // document properties

                Item {
                    Layout.fillHeight: true
                }

            }

        }
    }
}

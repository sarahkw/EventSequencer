import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0 as Qlp

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
            id: addMenu
            title: "Add"
            Action {
                text: "&Strip"
                onTriggered: {
                    var cppStrip = document.createStrip()
                    cppStrip.startFrame = cursor.frame
                    cppStrip.length = 10 // Maybe make this one large tick instead
                    selectedCppStrips = [cppStrip]
                }
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
        }

    }

    header: ToolBar {
        RowLayout {
            ToolButton {
                action: deleteAction
                ToolTip.visible: hovered
                ToolTip.text: action.shortcut
            }
            ToolButton {
                action: grabAction
                ToolTip.visible: hovered
                ToolTip.text: action.shortcut
            }
            ToolButton {
                action: selectAction
                ToolTip.visible: hovered
                ToolTip.text: action.shortcut
            }
        }
    }

    ES.Document {
        id: document
        framesPerSecond: 30
    }

    footer: ToolBar {
        Row {
            FrameTextField {
                document: document
                shouldShowTime: showSecondsAction.checked
                frame: cursor.frame
                onFrameEditingFinished: {
                    cursor.frame = frame
                }

                selectByMouse: true
                horizontalAlignment: TextInput.AlignRight
                ToolTip.text: showSecondsAction.checked ? "Current Time" : "Current Frame"
                ToolTip.visible: hovered
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



    Action {
        id: addAction
        text: "Add"
        onTriggered: addMenu.open()
        shortcut: "Shift+A"
    }

    Action {
        id: deleteAction
        text: "Delete"
        onTriggered: {
            selectedCppStrips.forEach(function (foo) {
                document.deleteStrip(foo)
            })
            selectedCppStrips = []
        }
        shortcut: "X"
    }

    Action {
        id: grabAction
        text: "Grab"
        checkable: true
        shortcut: "G"
    }

    Action {
        id: selectAction
        text: "Select"
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

            MouseArea {
                anchors.fill: bodyView
                acceptedButtons: Qt.RightButton
                onClicked: selectedCppStrips = []
            }

            Rectangle {
                id: bodyView
                anchors.left: sbVertHolder.right
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

                            Component.onCompleted: {
                                modelData.qmlStrip = strip
                            }

                            readonly property int startFrame: modelData.startFrame
                            readonly property int channel: modelData.channel
                            readonly property int length: modelData.length

                            x: zoom.mapFrameToDisplayX(startFrame)
                            width: Math.max(zoom.mapLengthToDisplayWidth(length), minimumWidth)
                            y: channel * channelPixels

                            selected: realIn(modelData, selectedCppStrips)

                            onSelectionClicked: {
                                if (mouse.modifiers & Qt.ShiftModifier) {
                                    var tmp = selectedCppStrips
                                    selectedCppStrips = []

                                    if (realIn(modelData, tmp)) {
                                        tmp = tmp.filter(function (foo) {
                                            return foo !== modelData;
                                        })
                                    } else {
                                        tmp.push(modelData)
                                    }

                                    selectedCppStrips = tmp
                                } else {
                                    selectedCppStrips = [modelData]
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
                                            modelData.startFrame += zoom.mapDisplayWidthToFullFrames(diffX)
                                            modelData.channel += floorDiv(diffY, channelPixels)
                                            break;
                                        case strip.selectionMode_RIGHT:
                                            modelData.length += zoom.mapDisplayWidthToFullFrames(diffX)
                                            break;
                                        case strip.selectionMode_LEFT:
                                            var initialEndFrame = startFrame + length
                                            modelData.length -= zoom.mapDisplayWidthToFullFrames(diffX)
                                            modelData.startFrame = initialEndFrame - length
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
                                            initialFrame: startFrame
                                            initialChannel: channel
                                        }

                                        PropertyChanges {
                                            target: modelData
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
                                            initialLength: length
                                        }

                                        PropertyChanges {
                                            target: modelData
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
                                            initialFrame: startFrame + length
                                            initialLength: length
                                        }

                                        PropertyChanges {
                                            target: modelData
                                            length: (initialLength -
                                                     zoom.mapDisplayWidthToFullFrames(grabMode.diffX))
                                            startFrame: initialFrame - length
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
                position: body.x
                displayFrameNumber: appwin.displayFrameNumber

                anchors.left: sbVertHolder.right
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }

            Item {
                id: sbVertHolder
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: sbHoriz.top
                width: 20
                /*
                Ruler {
                    id: sbVert
                    tickSize: channelPixels
                    position: body.y
                    height: parent.width
                    width: parent.height
                    y: -height

                    // Hack hack hack hack
                    rotation: 90
                    transformOrigin: Item.BottomLeft
                }
                */
            }

            Rectangle {
                id: sbDead
                anchors.left: parent.left
                anchors.top: sbVertHolder.bottom
                anchors.bottom: parent.bottom
                anchors.right: sbHoriz.left
                color: "gray"
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

        Rectangle {
            Drag.active: propertiesDraggerMouseArea.drag.active
            id: propertiesDragger
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            property int position: 200
            x: parent.width - position

            width: 2
            color: "white"

            Rectangle {
                color: "black"
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                width: 1
            }

            MouseArea {
                id: propertiesDraggerMouseArea
                anchors.fill: parent
                cursorShape: Qt.SizeHorCursor
                drag.target: propertiesDragger
                drag.axis: Drag.XAxis
            }

            onXChanged: {
                position = parent.width - x
            }

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

                Loader {
                    Layout.fillWidth: true
                    sourceComponent: selectedCppStrips.length == 1 ? propertiesComponent : blankComponent
                    property alias dokument: document

                    Component {
                        // Blank component so that the Loader resizes itself upon
                        // clearing.
                        id: blankComponent
                        Item {}
                    }

                    Component {
                        id: propertiesComponent
                        Column {
                            anchors.left: parent.left
                            anchors.right: parent.right

                            property var selectedThingar: selectedCppStrips[0]

                            Label {
                                text: "Edit Strip"
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
                                    text: selectedThingar.channel
                                    selectByMouse: true
                                    validator: IntValidator { }
                                    onEditingFinished: {
                                        selectedThingar.channel = parseInt(text, 10)
                                        focus = false
                                    }
                                }
                                Label {
                                    text: "Start"
                                }
                                FrameTextField {
                                    document: dokument
                                    shouldShowTime: showSecondsAction.checked
                                    frame: selectedThingar.startFrame
                                    onFrameEditingFinished: {
                                        selectedThingar.startFrame = frame
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
                                    frame: selectedThingar.length
                                    onFrameEditingFinished: {
                                        selectedThingar.length = frame
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
                                    frame: selectedThingar.startFrame + selectedThingar.length
                                    onFrameEditingFinished: {
                                        selectedThingar.length = frame - selectedThingar.startFrame
                                    }

                                    Layout.fillWidth: true
                                    selectByMouse: true
                                }
                            }
                            Item { // End Spacer
                                width: 1
                                height: 15
                            }

                        }
                    }
                }

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
                }

                Item {
                    Layout.fillHeight: true
                }

            }

        }
    }
}

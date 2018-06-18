import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

ApplicationWindow {
    id: appwin
    visible: true
    width: 800
    height: 600
    title: qsTr("Hello World")
    header: ToolBar {
        RowLayout {
            ToolButton {
                action: addAction
                ToolTip.visible: hovered
                ToolTip.text: action.shortcut
            }
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
            ToolButton {
                action: propertiesAction
                ToolTip.visible: hovered
                ToolTip.text: action.shortcut
            }
        }
    }

    footer: ToolBar {
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

    Action {
        id: addAction
        text: "Add"
        onTriggered: {
            var newThing = componentThing.createObject(body, {})
            selectedThingars = [newThing]
        }
        shortcut: "Shift+A"
    }

    Action {
        id: deleteAction
        text: "Delete"
        onTriggered: {
            selectedThingars.forEach(function (foo) {
                foo.destroy()
            })
            selectedThingars = []
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
            selectedThingars = []
            // TODO Also support Select All
        }
    }

    Action {
        id: propertiesAction
        text: "Properties"
        checkable: true
        shortcut: "N"
    }

    // "needle in haystack" doesn't seem to work for QML elements
    function realIn(needle, haystack) {
        return haystack.some(function (foo) { return foo === needle });
    }

    property var selectedThingars: []
    property int framePixels: 50
    property int channelPixels: 35

    Component {
        id: componentThing

        Thing {
            id: thingar

            property int startFrame: 0
            property int channel: 0
            property int length: 2
            
            x: startFrame * framePixels
            width: length * framePixels
            y: channel * channelPixels

            selected: realIn(thingar, selectedThingars)

            onSelectionClicked: {
                if (mouse.modifiers & Qt.ShiftModifier) {
                    var tmp = selectedThingars
                    selectedThingars = []

                    if (realIn(thingar, tmp)) {
                        tmp = tmp.filter(function (foo) {
                            return foo !== thingar;
                        })
                    } else {
                        tmp.push(thingar)
                    }

                    selectedThingars = tmp
                } else {
                    selectedThingars = [thingar]
                }
            }

            function floorDiv(a, b) {
                return Math.floor(a / b);
            }

            Connections {
                target: grabMode
                onFinalCommit: {
                    if (selected) {
                        switch (thingar.selectionMode) {
                        case thingar.selectionMode_WHOLE:
                            startFrame += floorDiv(diffX, framePixels)
                            channel += floorDiv(diffY, channelPixels)
                            break;
                        case thingar.selectionMode_RIGHT:
                            length += floorDiv(diffX, framePixels)
                            break;
                        case thingar.selectionMode_LEFT:
                            var initialEndFrame = startFrame + length
                            length -= floorDiv(diffX, framePixels)
                            startFrame = initialEndFrame - length
                            break;
                        }
                    }
                }
            }

            property int initialFrame
            property int initialChannel
            property int initialLength

            states: [
                State {
                    when: (selected &&
                           grabMode.grabState == grabMode.grabstate_MOVING &&
                           thingar.selectionMode == thingar.selectionMode_WHOLE)
                    PropertyChanges {
                        target: thingar
                        explicit: true
                        initialFrame: startFrame
                        initialChannel: channel
                    }

                    PropertyChanges {
                        target: thingar
                        startFrame: (initialFrame +
                                     floorDiv(grabMode.diffX, framePixels))
                        channel: (initialChannel +
                                  floorDiv(grabMode.diffY, channelPixels))
                    }
                },
                State {
                    when: (selected &&
                           grabMode.grabState == grabMode.grabstate_MOVING &&
                           thingar.selectionMode == thingar.selectionMode_RIGHT)
                    PropertyChanges {
                        target: thingar
                        explicit: true
                        initialLength: length
                    }

                    PropertyChanges {
                        target: thingar
                        length: (initialLength +
                                 floorDiv(grabMode.diffX, framePixels))
                    }
                },
                State {
                    when: (selected &&
                           grabMode.grabState == grabMode.grabstate_MOVING &&
                           thingar.selectionMode == thingar.selectionMode_LEFT)
                    PropertyChanges {
                        target: thingar
                        explicit: true
                        initialFrame: startFrame + length
                        initialLength: length
                    }

                    PropertyChanges {
                        target: thingar
                        length: (initialLength -
                                 floorDiv(grabMode.diffX, framePixels))
                        startFrame: initialFrame - length
                    }
                }
            ]
        }
    }

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
                onClicked: selectedThingars = []
            }

            Rectangle {
                id: bodyView
                anchors.left: sbVertHolder.right
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: sbHoriz.top

                // The fusion theme would make this gray otherwise.
                color: "white"

                Item {
                    id: body
                    implicitWidth: childrenRect.width + childrenRect.x
                    implicitHeight: childrenRect.height + childrenRect.y
                }
            }

            Ruler {
                id: sbHoriz
                height: 20
                tickSize: framePixels
                position: body.x

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
                    framePixels += whole
                    angleAccumulator = remain
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

            Loader {
                anchors.left: parent.left
                anchors.right: parent.right
                sourceComponent: selectedThingars.length == 1 ? propertiesComponent : undefined
                Component {
                    id: propertiesComponent
                    Column {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        property var selectedThingar: selectedThingars[0]

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
                                onEditingFinished: {
                                    selectedThingar.channel = parseInt(text, 10)
                                }
                            }
                            Label {
                                text: "Start Frame"
                            }
                            TextField {
                                Layout.fillWidth: true
                                text: selectedThingar.startFrame
                                selectByMouse: true
                                onEditingFinished: {
                                    selectedThingar.startFrame = parseInt(text, 10)
                                }
                            }
                            Label {
                                text: "Length"
                            }
                            TextField {
                                Layout.fillWidth: true
                                text: selectedThingar.length
                                selectByMouse: true
                                onEditingFinished: {
                                    selectedThingar.length = parseInt(text, 10)
                                }
                            }
                            Label {
                                text: "End Frame"
                            }
                            TextField {
                                Layout.fillWidth: true
                                text: selectedThingar.startFrame + selectedThingar.length
                                selectByMouse: true
                                onEditingFinished: {
                                    selectedThingar.length = parseInt(text, 10) - selectedThingar.startFrame
                                }
                            }

                        }
                    }
                }
            }

        }
    }
}

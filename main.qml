import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

ApplicationWindow {
    id: appwin
    visible: true
    width: 640
    height: 480
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
        onTriggered: componentThing.createObject(body, {})
        shortcut: "Shift+A"
    }

    Action {
        id: deleteAction
        text: "Delete"
        onTriggered: {
            selectedThingars.forEach(function (x) {
                x.destroy()
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

    // "needle in haystack" doesn't seem to work for QML elements
    function realIn(needle, haystack) {
        return haystack.some(function (x) { return x === needle });
    }

    property var selectedThingars: []

    Component {
        id: componentThing

        Thing {
            id: thingar

            selected: realIn(thingar, selectedThingars)

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onClicked: {
                    if (mouse.modifiers & Qt.ShiftModifier) {
                        var tmp = selectedThingars
                        selectedThingars = []

                        if (realIn(thingar, tmp)) {
                            tmp = tmp.filter(function (x) {
                                return x !== thingar;
                            })
                        } else {
                            tmp.push(thingar)
                        }

                        selectedThingars = tmp
                    } else {
                        selectedThingars = [thingar]
                    }
                }
            }

            function snapX(_x) {
                return Math.floor(_x / 50) * 50;
            }

            function snapY(_y) {
                return Math.floor(_y / 35) * 35
            }

            Connections {
                target: grabMode
                onFinalCommit: {
                    if (selected) {
                        x = snapX(x + diffX);
                        y = snapY(y + diffY);
                    }
                }
            }

            property int initialX
            property int initialY

            states: [
                State {
                    when: selected && grabMode.grabState == grabMode.grabstate_MOVING
                    PropertyChanges {
                        target: thingar
                        explicit: true
                        initialX: x
                        initialY: y
                    }

                    PropertyChanges {
                        target: thingar
                        x: snapX(initialX + grabMode.diffX)
                        y: snapY(initialY + grabMode.diffY)
                    }
                }
            ]
        }
    }

    MouseArea {
        anchors.fill: sView
        acceptedButtons: Qt.RightButton
        onClicked: selectedThingars = []
    }
    ScrollView {
        id: sView
        anchors.fill: parent
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn

        Rectangle {
            id: body
            implicitWidth: childrenRect.width + childrenRect.x
            implicitHeight: childrenRect.height + childrenRect.y
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
        anchors.fill: sView

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
                grabState = grabstate_MOVING; // XXX Only set if not already moving?
                break;
            }
        }
    }

}

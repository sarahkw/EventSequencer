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
                text: "Add"
                onClicked: {
                    componentThing.createObject(body, {})
                }
            }
        }
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

            Item {
                Drag.active: dragArea.drag.active

                id: dragProxy
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                MouseArea {
                    id: dragArea
                    anchors.fill: parent
                    drag.target: dragProxy
                }

                states: [
                    State {
                        when: dragArea.drag.active
                        PropertyChanges {
                            target: dragProxy
                            onXChanged: thingar.x = Math.floor(dragProxy.x / 50) * 50
                            onYChanged: thingar.y = Math.floor(dragProxy.y / 35) * 35
                        }
                        AnchorChanges {
                            target: dragProxy
                            anchors.left: undefined
                            anchors.right: undefined
                            anchors.top: undefined
                            anchors.bottom: undefined
                        }
                        ParentChange {
                            target: dragProxy
                            parent: thingar.parent
                        }
                    }
                ]
            }

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
        }
    }

    ScrollView {
        anchors.fill: parent
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: selectedThingars = []
        }
        Rectangle {
            id: body
            implicitWidth: childrenRect.width + childrenRect.x
            implicitHeight: childrenRect.height + childrenRect.y
        }
    }
}

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

    Component {
        id: componentThing
        Thing {
            Drag.active: dragArea.drag.active
            MouseArea {
                id: dragArea
                anchors.fill: parent
                drag.target: parent
            }
        }
    }

    ScrollView {
        anchors.fill: parent
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        Rectangle {
            id: body
            implicitWidth: childrenRect.width + childrenRect.x
            implicitHeight: childrenRect.height + childrenRect.y
            color: "#efefef"
        }
    }
}

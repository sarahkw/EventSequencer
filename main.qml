import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    header: ToolBar {
        RowLayout {
            ToolButton {
                text: "Add"
            }
        }
    }

    Thing {

    }
}

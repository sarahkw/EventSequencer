import QtQuick 2.0
import QtQuick.Controls 2.2
import "../util.js" as Util

Item {
    property Component stripPropertiesComponent: Component {
        Column {
            Label {
                text: "Content"
            }
            TextField {
                anchors.left: parent.left
                anchors.right: parent.right
                onEditingFinished: Util.mutableobjset(cppStrip.mutableLabel, "content", text)
                text: Util.nvlobjprop(cppStrip.label, "content", "")
            }
        }
    }
}

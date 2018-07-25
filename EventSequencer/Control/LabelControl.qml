import QtQuick 2.0
import QtQuick.Controls 2.2
import "../util.js" as Util
import "../" as Parent

Item {
    property Component stripPropertiesComponent: Component {
        Column {
            Label {
                text: "Content"
            }
            Parent.ESTextField {
                anchors.left: parent.left
                anchors.right: parent.right
                onEsEditingFinished: Util.mutableobjset(cppStrip.mutableLabel, "content", text)
                text: Util.nvlobjprop(cppStrip.label, "content", "")
            }
        }
    }
}

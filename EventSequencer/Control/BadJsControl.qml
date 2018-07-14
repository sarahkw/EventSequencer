import QtQuick 2.0
import QtQuick.Controls 2.2
import "../util.js" as Util

Item {
    property Component stripPropertiesComponent: Component {
        Column {
            Text {
                text: "BadJs to be eval()'ed:"
            }
            TextArea {
                anchors.left: parent.left
                anchors.right: parent.right
                selectByMouse: true
                background: Rectangle {
                    color: "white"
                }
                wrapMode: TextEdit.Wrap
                onEditingFinished: Util.mutableobjset(cppStrip.mutableBadJs, "script", text)
                text: Util.nvlobjprop(cppStrip.badJs, "script", "")
            }
        }
    }

    function playFrame(cppStrip, frame) {
        var script = Util.nvlobjprop(cppStrip.badJs, "script", "")
        eval(script);
    }
}

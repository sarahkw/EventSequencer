import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../util.js" as Util

Item {
    property Component stripPropertiesComponent: Component {
        Column {
            Label {
                text: "File:"
            }
            RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right

                TextField {
                    Layout.fillWidth: true
                    onEditingFinished: Util.mutableobjset(cppStrip.mutableAudio, "fileName", text)
                    text: Util.nvlobjprop(cppStrip.audio, "fileName", "")
                }
                Button {
                    text: "Browse"
                }
            }

        }
    }

}

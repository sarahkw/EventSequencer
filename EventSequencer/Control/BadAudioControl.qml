import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0 as Qlp
import "../util.js" as Util
import "../" as Parent

Item {
    property Component stripPropertiesComponent: Component {
        Column {
            Label {
                text: "File:"
            }
            RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right

                Parent.ESTextField {
                    Layout.fillWidth: true
                    text: Util.nvlobjprop(cppStrip.badAudio, "fileName", "")
                    onEsEditingFinished: Util.mutableobjset(cppStrip.mutableBadAudio, "fileName", text)
                }
                Button {
                    text: "Browse"
                    onClicked: fopenDialog.open()

                    Qlp.FileDialog {
                        id: fopenDialog
                        onAccepted: Util.mutableobjset(cppStrip.mutableBadAudio, "fileNameUrl", currentFile)
                    }
                }
            }
        }
    }
}

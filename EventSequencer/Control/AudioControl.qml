import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0 as Qlp
import "../util.js" as Util
import "../" as Parent
import eventsequencer 1.0 as ES

Item {
    property bool willAcceptResource: true

    property Component stripPropertiesComponent: Component {
        GridLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            columns: 2

            Label { text: "Resource" }
            RowLayout {
                Parent.ESTextField {
                    Layout.fillWidth: true
                    text: cppStrip !== null ? cppStrip.resourceUrl : ""
                    onEsEditingFinished: cppStrip.resourceUrl = text
                }
                Button {
                    text: "Browse"
                    onClicked: fopenDialog.open()

                    Qlp.FileDialog {
                        id: fopenDialog
                        onAccepted: cppStrip.resourceUrl = currentFile
                    }
                }
            }
        }
    }
}

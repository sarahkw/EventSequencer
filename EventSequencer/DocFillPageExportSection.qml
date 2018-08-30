import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

GroupBox {
    property alias description: descriptionLabel.text

    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        Label {
            id: descriptionLabel
            Layout.fillWidth: true
            wrapMode: Text.Wrap
        }
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "Output Path"
            }
            TextField {
                Layout.fillWidth: true
                readOnly: !btnUseCustomPath.checked
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Button {
                id: btnUseCustomPath
                Layout.fillWidth: true
                checkable: true
                text: "Use Custom Path"
            }
            Button {
                Layout.fillWidth: true
                text: "Export"
            }
        }
    }
}

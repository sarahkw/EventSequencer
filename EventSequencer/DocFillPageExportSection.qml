import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

GroupBox {
    property alias description: descriptionLabel.text
    property string defaultOutputPath
    property alias outputPath: txtOutputPath.text
    property bool defaultOutputPathExists
    property bool actionEnabled: true
    property bool showPreviewAndUpdate: false

    signal exportActivated()
    signal deleteActivated()
    signal previewActivated()
    signal updateActivated()

    states: [
        State {
            when: defaultOutputPathExists
            PropertyChanges {
                target: btnExport
                text: "Delete"
                onActivated: deleteActivated()
            }
            PropertyChanges {
                target: btnPreview
                visible: showPreviewAndUpdate
            }
            PropertyChanges {
                target: btnUpdate
                visible: showPreviewAndUpdate
            }
        }
    ]

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
                text: "Path"
            }
            TextField {
                Layout.fillWidth: true
                id: txtOutputPath
                readOnly: true
                text: defaultOutputPath
                selectByMouse: true
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Button {
                id: btnPreview
                Layout.fillWidth: true
                text: "Preview"
                onClicked: previewActivated()
                visible: false
                enabled: actionEnabled
            }
            DelayButton {
                id: btnUpdate
                Layout.fillWidth: true
                text: "Update"
                onActivated: updateActivated()
                onCheckedChanged: checked = false
                visible: false
                enabled: actionEnabled
            }
            DelayButton {
                id: btnExport
                Layout.fillWidth: true
                text: "Export"
                onActivated: exportActivated()
                onCheckedChanged: checked = false
                enabled: actionEnabled
            }
        }
    }
}

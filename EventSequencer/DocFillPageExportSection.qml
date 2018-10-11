import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQml.Models 2.3

GroupBox {
    property alias description: descriptionLabel.text
    property string defaultOutputPath
    property alias outputPath: txtOutputPath.text
    property bool defaultOutputPathExists
    property bool actionEnabled: true
    property bool showPreviewAndUpdate: false
    property bool mp3optionsActive: false

    function getMp3Quality() {
        if (mp3optionsLoader.item !== null) {
            return mp3optionsLoader.item.getMp3Quality()
        } else {
            throw new Error("mp3 quality doesn't exist");
        }
    }

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
        GridLayout {
            Layout.fillWidth: true
            columns: 2
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
            Repeater {
                model: mp3optionsLoader.item
            }
            Loader {
                id: mp3optionsLoader
                active: mp3optionsActive
                sourceComponent: ObjectModel {
                    function getMp3Quality() {
                        return cmbMp3Quality.currentIndex
                    }

                    Label {
                        text: "Quality"
                    }
                    ComboBox {
                        Layout.fillWidth: true
                        id: cmbMp3Quality
                        model: [
                            'V0 (Highest quality)',
                            'V1',
                            'V2',
                            'V3',
                            'V4',
                            'V5',
                            'V6 (Default)',
                            'V7',
                            'V8',
                            'V9 (Smallest file)'
                        ]
                        currentIndex: 6
                        enabled: actionEnabled
                    }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Button {
                id: btnPreview
                Layout.fillWidth: true
                text: "View"
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

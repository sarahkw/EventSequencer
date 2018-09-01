import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2

import eventsequencer 1.0 as ES

Item {
    property string unassignedFileName

    ES.ManagedResourceReport {
        id: report
        ignoredFileNames: [
            unassignedFileName
        ]
    }

    states: [
        State {
            when: report.hasData
            PropertyChanges {
                target: btnGenerateReport
                text: "Clear Report"
                onClicked: report.clearReport()
            }
        }
    ]

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: width
            clip: true
            Column {
                visible: report.hasData
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 15
                GroupBox {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    title: "Information"
                    GridLayout {
                        columns: 2

                        Label { text: "Active Resource Size" }
                        Label {
                            Layout.fillWidth: true
                            text: report.usedFilesSize
                        }

                        Label { text: "Orphan Resource Size" }
                        Label {
                            Layout.fillWidth: true
                            text: report.unusedFilesSize
                        }
                    }
                }
                GroupBox {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    title: "Orphaned Resources"
                    ColumnLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        Repeater {
                            model: report.unusedFiles
                            Text {
                                text: modelData
                                Layout.fillWidth: true
                            }
                        }
                        Text {
                            text: "Nothing"
                            font.italic: true
                            visible: report.unusedFiles.length === 0
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Button {
                                Layout.fillWidth: true
                                text: "Copy List"
                                onClicked: {
                                    ES.ClipboardWrapper.setText(report.unusedFilesAsText())
                                }
                            }
                            Button {
                                Layout.fillWidth: true
                                text: "Delete All Files"

                                MessageDialog {
                                    id: mdConfirmDeleteAllFiles
                                    standardButtons: StandardButton.Yes | StandardButton.No
                                    text: "Are you sure you wish to delete all orphaned files?"
                                    onYes: {
                                    }
                                }

                                onClicked: mdConfirmDeleteAllFiles.open()
                            }
                        }
                    }
                }

                GroupBox {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    title: "Strips Missing Resources"
                    ColumnLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        Repeater {
                            model: report.stripsMissingResource
                            Button {
                                clip: true
                                contentItem: ColumnLayout {
                                    Text {
                                        Layout.fillWidth: true
                                        text: "%1: %2".arg(modelData.startFrame).arg(managedResources.urlConvertToFileName(modelData.resourceUrl))
                                    }
                                }
                                onClicked: goToAndSelectStrip(modelData)
                                Layout.fillWidth: true
                            }
                        }
                        Text {
                            text: "Nothing"
                            font.italic: true
                            visible: report.stripsMissingResource.length === 0
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Button {
                                Layout.fillWidth: true
                                text: "Copy List"
                                onClicked: {
                                    ES.ClipboardWrapper.setText(report.stripsMissingResourceAsText())
                                }
                            }
                            Button {
                                Layout.fillWidth: true
                                text: "Delete All Strips"

                                MessageDialog {
                                    id: mdConfirmDeleteAllStrips
                                    standardButtons: StandardButton.Yes | StandardButton.No
                                    text: "Are you sure you wish to delete all strips that are missing resources?"
                                    onYes: {
                                        report.deleteAllStripsMissingResources(document)
                                        autoSaveManager.markDirty()
                                    }
                                }

                                onClicked: {
                                    mdConfirmDeleteAllStrips.open()
                                }
                            }
                        }
                    }
                }
            }
        }
        Button {
            id: btnGenerateReport
            Layout.fillWidth: true
            text: "Generate Report"
            onClicked: {
                report.generateReport(document)
            }
        }
    }
}

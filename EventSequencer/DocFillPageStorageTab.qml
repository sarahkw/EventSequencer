import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES

Item {
    ES.ManagedResourceReport {
        id: report
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
                        Label { text: "Report Age" }
                        Label {
                            Layout.fillWidth: true
                            text: "2 minutes ago"
                        }

                        Label { text: "Active Resource Size" }
                        Label {
                            Layout.fillWidth: true
                            text: "80 MB"
                        }

                        Label { text: "Orphan Resource Size" }
                        Label {
                            Layout.fillWidth: true
                            text: "10 MB"
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
                                text: modelData + ""
                                Layout.fillWidth: true
                            }
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Button {
                                Layout.fillWidth: true
                                text: "Copy List"
                            }
                            Button {
                                Layout.fillWidth: true
                                text: "Delete All"
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
                            Text {
                                text: modelData + ""
                                Layout.fillWidth: true
                            }
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Button {
                                Layout.fillWidth: true
                                text: "Copy List"
                            }
                            Button {
                                Layout.fillWidth: true
                                text: "Delete All"
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

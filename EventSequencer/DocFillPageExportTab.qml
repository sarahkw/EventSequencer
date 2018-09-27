import QtQuick 2.0
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES

ScrollView {
    contentWidth: width
    Column {
        id: unnamedParent_6e26
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 15
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        spacing: 15

        property var document_rebind: document

        ES.DocFillExportManager {
            id: exportManager
            document: unnamedParent_6e26.document_rebind

            // Can be undefined before Android service provides status.
            property bool watchForStopWorking: batchServiceStatus !== undefined ? batchServiceStatus.isWorking : false
            onWatchForStopWorkingChanged: {
                if (!watchForStopWorking) {
                    // Just stopped working.
                    tempUpdateDefaultOutputPaths()
                }
            }
        }

        GroupBox {
            anchors.left: parent.left
            anchors.right: parent.right
            title: "Worker Status"
            Label {
                anchors.left: parent.left
                anchors.right: parent.right
                text: "%1".arg((function () {
                    if (exportManager.batchServiceStatus === undefined) {
                        return "Offline"
                    } else if (!exportManager.batchServiceStatus.isWorking) {
                        if (exportManager.batchServiceStatus.statusText !== "") {
                            return "Finished: %1".arg(exportManager.batchServiceStatus.statusText)
                        } else {
                            return "Idle"
                        }
                    } else {
                        return exportManager.batchServiceStatus.statusText
                    }
                })())
            }
        }

        DocFillPageExportSection {
            anchors.left: parent.left
            anchors.right: parent.right
            title: "Export HTML"
            description: "Write a HTML file with MP3-encoded resources for you to share on the world wide web."
            defaultOutputPath: exportManager.defaultExportHtmlOutputPath
            defaultOutputPathExists: exportManager.defaultExportHtmlOutputPathExists
            onExportActivated: {
                var result = exportManager.requestExportHtml(document)
                if (result !== "") {
                    msgbox.msgbox(result, "Export")
                }
            }
            onDeleteActivated: {
                if (!exportManager.deleteDefaultExportHtmlOutputPath()) {
                    msgbox.msgbox("Delete failure", "Export")
                }
            }
        }

        DocFillPageExportSection {
            anchors.left: parent.left
            anchors.right: parent.right
            title: "Export JSON"
            description: "Write a JSON file that lists the parts of the document along with the assigned audio file."
            defaultOutputPath: exportManager.defaultExportJsonOutputPath
            defaultOutputPathExists: exportManager.defaultExportJsonOutputPathExists
            onExportActivated: {
                var result = exportManager.requestExportJson(document)
                if (result !== "") {
                    msgbox.msgbox(result, "Export")
                }
            }
            onDeleteActivated: {
                if (!exportManager.deleteDefaultExportJsonOutputPath()) {
                    msgbox.msgbox("Delete failure", "Export")
                }
            }
        }

        DocFillPageExportSection {
            anchors.left: parent.left
            anchors.right: parent.right
            title: "Play to File"
            description: "Concatenate all individual recordings into a single audio file."
            defaultOutputPath: exportManager.defaultPlayToFileOutputPath
            defaultOutputPathExists: exportManager.defaultPlayToFileOutputPathExists
            onExportActivated: {
                var result = exportManager.requestExportPlayToFile(document)
                if (result !== "") {
                    msgbox.msgbox(result, "Export")
                }
            }
            onDeleteActivated: {
                if (!exportManager.deleteDefaultPlayToFileOutputPath()) {
                    msgbox.msgbox("Delete failure", "Export")
                }
            }
        }
    }
}

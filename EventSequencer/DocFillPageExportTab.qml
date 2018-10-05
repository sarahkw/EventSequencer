import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

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
            property bool serviceIsWorking: batchServiceStatus !== undefined ? batchServiceStatus.isWorking : false
            onServiceIsWorkingChanged: {
                if (!serviceIsWorking) {
                    // Just stopped working.
                    tempUpdateDefaultOutputPaths()
                }
            }
        }

        GroupBox {
            anchors.left: parent.left
            anchors.right: parent.right
            title: "Export: %1".arg((function () {
                if (exportManager.batchServiceStatus === undefined) {
                    return ""
                }
                return exportManager.batchServiceStatus.fileName
            })())
            visible: (lblStatusText.text !== "" || exportManager.serviceIsWorking)
            RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                Label {
                    Layout.fillWidth: true
                    id: lblStatusText
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: {
                        if (exportManager.batchServiceStatus === undefined) {
                            return ""
                        }
                        return exportManager.batchServiceStatus.statusText
                    }
                }
                Button {
                    id: btnAction
                    text: "Ok"
                    onClicked: exportManager.requestClearStatus()
                    states: [
                        State {
                            when: exportManager.serviceIsWorking
                            PropertyChanges {
                                target: btnAction
                                text: "Cancel"
                                onClicked: exportManager.requestCancelWorker()
                            }
                        }
                    ]
                }
            }
        }

        DocFillPageExportSection {
            anchors.left: parent.left
            anchors.right: parent.right
            title: "Export HTML"
            description: "Write a HTML file with MP3-encoded resources for you to share on the world wide web."
            defaultOutputPath: exportManager.defaultExportHtmlOutputPath
            defaultOutputPathExists: exportManager.defaultExportHtmlOutputPathExists
            actionEnabled: !exportManager.serviceIsWorking
            showPreviewAndUpdate: true
            onUpdateActivated: {
                var result = exportManager.requestExportHtml(document, true)
                if (result !== "") {
                    msgbox.msgbox(result, "Update")
                }
            }
            onExportActivated: {
                var result = exportManager.requestExportHtml(document, false)
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
            title: "Play to File"
            description: "Concatenate all individual recordings into a single audio file."
            defaultOutputPath: exportManager.defaultPlayToFileOutputPath
            defaultOutputPathExists: exportManager.defaultPlayToFileOutputPathExists
            actionEnabled: !exportManager.serviceIsWorking
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

        DocFillPageExportSection {
            anchors.left: parent.left
            anchors.right: parent.right
            title: "Export JSON"
            description: "Write a JSON file that lists the parts of the document along with the assigned audio file."
            defaultOutputPath: exportManager.defaultExportJsonOutputPath
            defaultOutputPathExists: exportManager.defaultExportJsonOutputPathExists
            actionEnabled: !exportManager.serviceIsWorking
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
    }
}

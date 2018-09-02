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
        }

        DocFillPageExportSection {
            anchors.left: parent.left
            anchors.right: parent.right
            title: "Export JSON"
            description: "Write a JSON file that lists the parts of the document along with the assigned audio file."
            defaultOutputPath: exportManager.defaultExportJsonOutputPath
            defaultOutputPathExists: exportManager.defaultExportJsonOutputPathExists
            onExportActivated: {
                var result = exportManager.exportJson(cppTextChannel, cppResourceChannel)
                msgbox.msgbox(result, "Export")
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
            description: "Play as configured in the 'Play' tab, but output to a file instead of to your speakers.

Current selection: %1".arg(cmbSelectionMode.currentText)
            defaultOutputPath: exportManager.defaultPlayToFileOutputPath
            defaultOutputPathExists: exportManager.defaultPlayToFileOutputPathExists
            onExportActivated: {
                var result = exportManager.exportPlayToFile(playerControl.playable)
                msgbox.msgbox(result, "Export")
            }
            onDeleteActivated: {
                if (!exportManager.deleteDefaultPlayToFileOutputPath()) {
                    msgbox.msgbox("Delete failure", "Export")
                }
            }
        }
    }
}

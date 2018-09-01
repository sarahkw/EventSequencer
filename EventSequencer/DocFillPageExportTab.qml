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
        }

        DocFillPageExportSection {
            anchors.left: parent.left
            anchors.right: parent.right
            title: "Play to File"
            description: "Play as configured in the 'Play' tab, but output to a file instead of to your speakers."
            defaultOutputPath: exportManager.defaultPlayToFileOutputPath
        }
    }
}

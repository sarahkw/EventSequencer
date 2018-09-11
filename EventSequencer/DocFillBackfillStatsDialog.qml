import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import eventsequencer 1.0 as ES

Dialog {
    id: root
    title: "Backfill Stats"

    property alias filePath: dfbs.filePath
    property var returnResult // fn

    onClosed: destroy()

    ES.DocFillBackfillStats {
        id: dfbs
        docfillDatabase: docFillDatabase // TODO fix case
    }

    parent: ApplicationWindow.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: parent.width * 0.97
    height: parent.height * 0.5

    modal: true
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Yes | Dialog.No

    // Hack: without this, Dialog will have a binding loop trying to size itself
    //       to the single item.
    Item {}

    ColumnLayout {
        anchors.fill: parent
        Label {
            Layout.fillWidth: true
            text: "Do you wish to add this document's stats to the database? If the stats are already there, it will be counted again."
            wrapMode: Text.Wrap
        }
        ScrollView {
            id: scrollview
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentWidth: width
            TextEdit {
                id: txtEdit
                width: scrollview.width
                readOnly: true
                wrapMode: TextEdit.Wrap
                text: dfbs.report
                font.family: platformMonospacedFont
            }
        }
    }
}

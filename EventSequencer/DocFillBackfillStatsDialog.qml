import QtQuick 2.0
import QtQuick.Controls 2.2
import eventsequencer 1.0 as ES

Dialog {
    id: root
    title: "Backfill Stats"

    onClosed: destroy()

    ES.DocFillBackfillStats {
        id: dfbs
    }

    parent: ApplicationWindow.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: parent.width * 0.97
    //height: parent.height * 0.97

    modal: true
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Yes | Dialog.No

    Label {
        anchors.left: parent.left
        anchors.right: parent.right
        text: "Do you wish to add this document's statistics to the database? Nothing stops you from adding the same statistics twice."
        wrapMode: Text.Wrap
    }
}

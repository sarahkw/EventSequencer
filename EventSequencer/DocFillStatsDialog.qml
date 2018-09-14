import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2 as NativeDialogs
import eventsequencer 1.0 as ES

Dialog {
    id: root
    title: "Stats"

    onClosed: destroy()

    parent: ApplicationWindow.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: parent.width * 0.97
    height: parent.height * 0.97

    modal: true
    closePolicy: Popup.CloseOnEscape

    NativeDialogs.MessageDialog {
        id: confirmResetDialog
        title: "Reset Stats"
        text: "Are you sure you wish to erase all statistics?"
        standardButtons: NativeDialogs.StandardButton.Yes | NativeDialogs.StandardButton.No
        onYes: {
            docFillDatabase.statsReset()
            txtEdit.reload()
        }
    }

    ES.QmlDialogButtonOrderHack {
        id: qdboh
        buttonCount: 3
        onButtonInit: {
            buttonList[0].text = "Copy"
            buttonList[1].text = "Reset"
            buttonList[2].text = "Close"
        }
        onButtonClicked: {
            switch (index) {
            case 0: txtEdit.selectAll(); txtEdit.copy(); txtEdit.deselect(); break
            case 1: confirmResetDialog.open(); break
            case 2: root.close(); break
            }
        }
    }
    footer: DialogButtonBox {
        Repeater {
            model: qdboh.buttonCount
            Button {
                property bool reg: qdboh.registerButton(this);
                onClicked: qdboh.registerButtonClick(this)
            }
        }
    }

    ScrollView {
        id: scrollView
        clip: true
        anchors.fill: parent
        contentWidth: width
        TextEdit {
            id: txtEdit
            width: scrollView.width
            readOnly: true
            wrapMode: TextEdit.Wrap
            text: docFillDatabase.statsGenerateReport()
            function reload() {
                text = docFillDatabase.statsGenerateReport()
            }
            font.family: platformMonospacedFont
        }
    }
}

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES

GridLayout {
    columns: 2
    Label { text: "Forked From" }
    ESTextField {
        Layout.fillWidth: true
        readOnly: !propertiesForkEdit.allowForkedFromEdit
        text: document.fileForkedFromChecksum
        onEsEditingFinished: document.fileForkedFromChecksum = text
    }
    Label { text: "Fork Action" }
    ComboBox {
        id: propertiesForkEdit
        Layout.fillWidth: true
        ToolTip.visible: hovered
        ToolTip.text: "Action to take during next save"
        model: ["", "Auto Set", "Clear", "User Edit"]
        readonly property bool allowForkedFromEdit: currentIndex === 3
        onActivated: {
            switch (index) {
            case 0: document.fileForkEditFlag = ES.Document.ForkEditFlag.None; break
            case 1: document.fileForkEditFlag = ES.Document.ForkEditFlag.Set; break
            case 2: document.fileForkEditFlag = ES.Document.ForkEditFlag.Clear; break
            case 3: document.fileForkEditFlag = ES.Document.ForkEditFlag.Custom; break
            }
            focus = false
        }
        currentIndex: {
            switch (document.fileForkEditFlag) {
            case ES.Document.ForkEditFlag.None  : return 0
            case ES.Document.ForkEditFlag.Set   : return 1
            case ES.Document.ForkEditFlag.Clear : return 2
            case ES.Document.ForkEditFlag.Custom: return 3
            }
        }
    }
    Label { text: "Resource Dir" }
    ESTextField {
        Layout.fillWidth: true
        readOnly: true
        text: document.fileResourceDirectory
    }
}

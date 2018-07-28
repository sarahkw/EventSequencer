import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0 as Qlp
import "../util.js" as Util
import "../" as Parent
import eventsequencer 1.0 as ES

Item {
    property Component stripPropertiesComponent: Component {
        GridLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            columns: 2

            Label { text: "Resource" }
            RowLayout {
                ComboBox {
                    id: cmbResourceType
                    Layout.fillWidth: true
                    model: ["", "Filesystem", "Managed"]
                    currentIndex: {
                        switch (cppStrip.resource.type) {
                        case ES.Resource.Type.None: return 0
                        case ES.Resource.Type.FilePath: return 1
                        case ES.Resource.Type.ManagedId: return 2
                        }
                    }
                    readonly property bool shouldShowIdentifier: currentIndex !== 0
                    readonly property bool shouldAllowBrowse: currentIndex === 1
                    onCurrentIndexChanged: {
                        switch (currentIndex) {
                        case 0:
                            cppStrip.resource.type = ES.Resource.Type.None
                            break
                        case 1:
                            cppStrip.resource.type = ES.Resource.Type.FilePath
                            break
                        case 2:
                            cppStrip.resource.type = ES.Resource.Type.ManagedId
                            break
                        }
                    }
                    onActivated: {
                        cppStrip.resource.identifier = ""
                    }
                }
                Button {
                    text: "Browse"
                    onClicked: fopenDialog.open()
                    enabled: cmbResourceType.shouldAllowBrowse

                    Qlp.FileDialog {
                        id: fopenDialog
                        onAccepted: cppStrip.resource.setFilePathFromUrl(currentFile)
                    }
                }
            }

            Label {
                text: "\u2514 Identifier"
                visible: cmbResourceType.shouldShowIdentifier
            }
            Parent.ESTextField {
                Layout.fillWidth: true
                visible: cmbResourceType.shouldShowIdentifier
                text: cppStrip.resource.identifier
                onEsEditingFinished: cppStrip.resource.identifier = text
            }

        }
    }
}

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4

// This sucks, a lot.
import QtQuick.Controls.Fusion 2.4
import QtQuick.Controls.Fusion.impl 2.4

Menu {
    delegate: MenuItem {
        id: control
        contentItem: RowLayout {
            // TODO: Should rip off more features from ~/Qt/5.11.0/Src/qtquickcontrols2/src/imports/controls/fusion/MenuItem.qml
            Label {
                Layout.fillWidth: true
                text: control.text
                color: control.down || control.highlighted ? Fusion.highlightedText(control.palette) : control.palette.text
            }
            Label {
                text: control.action.shortcut
                color: control.down || control.highlighted ? Fusion.highlightedText(control.palette) : control.palette.text
            }
        }
    }
}

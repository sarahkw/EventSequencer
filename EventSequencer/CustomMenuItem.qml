import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4

// This sucks, a lot.
import QtQuick.Controls.impl 2.4
import QtQuick.Controls.Fusion 2.4
import QtQuick.Controls.Fusion.impl 2.4

MenuItem {
    id: control
    contentItem: RowLayout {
        // Copied from ~/Qt/5.11.0/Src/qtquickcontrols2/src/imports/controls/fusion/MenuItem.qml
        IconLabel {
            Layout.fillWidth: true

            readonly property real arrowPadding: control.subMenu && control.arrow ? control.arrow.width + control.spacing : 0
            readonly property real indicatorPadding: control.checkable && control.indicator ? control.indicator.width + control.spacing : 0
            leftPadding: !control.mirrored ? indicatorPadding : arrowPadding
            rightPadding: control.mirrored ? indicatorPadding : arrowPadding

            spacing: control.spacing
            mirrored: control.mirrored
            display: control.display
            alignment: Qt.AlignLeft

            icon: control.icon
            text: control.text
            font: control.font
            color: control.down || control.highlighted ? Fusion.highlightedText(control.palette) : control.palette.text
        }
        Label {
            text: control.action && control.action.shortcut || ""
            color: control.down || control.highlighted ? Fusion.highlightedText(control.palette) : control.palette.text
        }
    }
}

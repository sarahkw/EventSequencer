import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.2

import eventsequencer 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Document View")
    flags: Qt.Dialog

    property bool destroyOnHide: false
    onVisibleChanged: {
        if (destroyOnHide && visible === false) {
            destroy()
        }
    }

    property string crazyText: "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.

Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

    ConstrainedMetricsFontUtil {
        id: cmfu
        constrainByWidthValue: 10
    }

    WordWrappedTextTrack {
        id: wwtt
        width: destRect.width - cmfu.constrainByWidthValue /*For wrapped space or NewLine*/
        text: crazyText
        font: cmfu.builtFont
    }

    Rectangle {
        id: destRect
        border.width: 1

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.topMargin: 5
        anchors.bottomMargin: 5

        ListView {
            anchors.fill: parent
            model: wwtt
            delegate: Item {
                height: cmfu.builtFontHeight * 2
                Text {
                    text: modelData
                    font: cmfu.builtFont
                }
                Rectangle {
                    y: cmfu.builtFontHeight
                    height: cmfu.builtFontHeight
                    width: modelData.length * cmfu.constrainByWidthValue
                    color: "pink"
                }
            }
        }
    }
}

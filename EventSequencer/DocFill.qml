import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import eventsequencer 1.0 as ES

ApplicationWindow {
    id: root
    visible: true
    width: 600
    height: 800
    title: qsTr("DocFill")
    flags: Qt.Dialog

    property var cppChannel
    onCppChannelChanged: {
        if (cppChannel == null) {
            root.destroy()
        }
    }

    property ES.Document document
    property int cursorFrame
    property var changeCursorFrame // Fn

    property bool destroyOnHide: false
    onVisibleChanged: {
        if (destroyOnHide && visible === false) {
            destroy()
        }
    }

    ES.ErrorReportingContext {
        id: errorReportingContext
    }

    header: TabBar {
        id: tbar
        currentIndex: sview.currentIndex

        TabButton { text: "Seek" }
        TabButton { text: "Record" }
        TabButton { text: "Selection" }
        TabButton { text: "Properties" }
        TabButton { text: "EVSEQ" }
        TabButton { text: "File" }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Repeater {
            model: errorReportingContext.model
            ErrorMessageNagDelegate {
                Layout.fillWidth: true
            }
        }

        DocumentViewControl {
            Layout.fillWidth: true
            Layout.fillHeight: true

            id: dvc
            document: root.document
            cursorFrame: root.cursorFrame
            changeCursorFrame: root.changeCursorFrame

            textChannelIndex: cppChannel.textChannel
            renderChannelIndex: cppChannel.resourceChannel

            ES.ConditionalError {
                errorReportingContext: errorReportingContext
                active: !dvc.textChannelIsValid
                errorText: "Invalid DocFill Text Channel"
            }

            ES.ConditionalError {
                errorReportingContext: errorReportingContext
                active: !dvc.renderChannelIsValid
                errorText: "Invalid DocFill Resource Channel"
            }
        }

    }

    footer: Frame {
        anchors.left: parent.left
        anchors.right: parent.right
        SwipeView {
            id: sview
            currentIndex: tbar.currentIndex
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            // Take implicitHeight from a normal item
            implicitHeight: recordButtons.implicitHeight

            RowLayout {}

            RowLayout {
                id: recordButtons
                Button {
                    Layout.fillWidth: true
                    text: "Delete"
                }
                Button {
                    Layout.fillWidth: true
                    text: "Assign"
                }
                Button {
                    Layout.fillWidth: true
                    text: "Preview"
                }
                Button {
                    Layout.fillWidth: true
                    text: "Start/Stop"
                }
            }

            RowLayout {
                Button {
                    Layout.fillWidth: true
                    text: "Start to Cursor"
                }
                Button {
                    Layout.fillWidth: true
                    text: "End to Cursor"
                }
                Button {
                    Layout.fillWidth: true
                    text: "Delete"
                }
            }
            RowLayout {}
            RowLayout {}
            RowLayout {}
        }
    }
}

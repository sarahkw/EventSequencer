import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import eventsequencer 1.0 as ES

ApplicationWindow {
    flags: Qt.Dialog
    width: 640
    height: 480

    ES.ErrorReportingContext {
        id: errorReportingContext
    }

    header: Frame {
        RowLayout {
            CheckBox {
                id: unnamedParent_67d6
                text: "Error 1"
                ES.ConditionalError {
                    errorReportingContext: errorReportingContext
                    active: unnamedParent_67d6.checked
                    errorText: "Error 1 is checked"
                }
            }
            CheckBox {
                id: unnamedParent_537b
                text: "Error 2"
                ES.ConditionalError {
                    errorReportingContext: errorReportingContext
                    active: unnamedParent_537b.checked
                    errorText: "Error 2 is checked"
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        Repeater {
            model: errorReportingContext.model
            ErrorMessageNagDelegate {
                Layout.fillWidth: true
            }
        }
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "white"
        }
    }

}

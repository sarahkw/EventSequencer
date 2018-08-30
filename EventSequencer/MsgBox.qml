import QtQuick 2.0
import QtQuick.Dialogs 1.2

MessageDialog {
    id: msgbox
    function msgbox(msg, title_) {
        if (visible) {
            // TODO Come up with a way of queueing msgbox's/error msgs
            console.error("MsgBox cannot show message", title_, msg)
        }
        if (title_) {
            title = title_
        } else {
            title = null
        }
        text = msg
        open()
    }
}

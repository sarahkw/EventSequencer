import QtQuick 2.0
import QtQuick.Dialogs 1.2

MessageDialog {
    id: msgbox
    function msgbox(msg, title_) {
        if (visible) {
            console.warn("Replacing unacknowledged message", text)
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

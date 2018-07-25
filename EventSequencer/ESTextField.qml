import QtQuick.Controls 2.2

TextField {
    signal esEditingFinished()
    selectByMouse: true
    onEditingFinished: {
        esEditingFinished()
        focus = false
    }
}

import QtQuick 2.0

Item {
    id: root

    property bool autoSaveEnabled: false

    property bool isDirty: false
    signal saveNow()

    function markDirty() {
        root.isDirty = true
        timer.restart()
    }
    function markManualSaved() {
        root.isDirty = false
        timer.stop()
    }

    Timer {
        id: timer
        interval: 500
        onTriggered: {
            if (root.autoSaveEnabled) {
                root.saveNow()
                root.isDirty = false
            }
        }
    }
}

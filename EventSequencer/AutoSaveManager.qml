import QtQuick 2.0

Item {
    id: root

    property bool autoSaveEnabled: false

    property bool isDirty: false

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
        // Basically save immediately after, on the next event loop
        // run. But delay long enough to flash the auto-save
        // asterisk. (Animation timer for QML is 60fps)
        interval: 40
        onTriggered: {
            if (root.autoSaveEnabled) {
                if (saveFn()) {
                    root.isDirty = false
                }
            }
        }
    }
}

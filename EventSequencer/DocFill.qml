import QtQuick 2.9
import QtQuick.Window 2.3

import eventsequencer 1.0 as ES

Window {
    id: root
    visible: true
    width: 540
    height: 720
    title: qsTr("DocFill")

    // Pass to DocFillPage Begin
    property var cppChannel
    property ES.Session session
    property ES.Document document
    property int cursorFrame
    property var changeCursorFrame // Fn
    // Pass to DocFillPage End

    Connections {
        target: cppChannel
        onBeforeDelete: {
            // If cppChannel goes away, delete ourselves immediately so that
            // we don't resolve bindings on "cppChannel" which will be null.
            // Calling "destroy" deletes not soon enough.
            //
            // Note: this assumes that DocFill was created dynamically.
            immediateDestructor.add(root)
        }
    }

    property bool destroyOnHide: false
    onVisibleChanged: {
        if (destroyOnHide && visible === false) {
            destroy()
        }
    }

    DocFillPage {
        anchors.fill: parent
        function closeFn() {
            root.destroy()
        }
    }
}

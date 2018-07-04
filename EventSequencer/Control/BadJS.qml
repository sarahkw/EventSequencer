import QtQuick 2.0

Item {
    property var roles: []
    property Component stripPropertiesComponent: Component {
        Text {
            text: "Hello from BadJS!"
        }
    }
}

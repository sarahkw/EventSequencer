import QtQuick 2.0

Item {
    property var roles: []
    property Component channelPropertiesComponent: Component {
        Text {
            text: "Hello from Text properties!"
        }
    }
}

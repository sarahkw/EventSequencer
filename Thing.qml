import QtQuick 2.0

Rectangle {
    property bool selected: false

    color: selected ? "black" : "red"
    width: 100
    height: 20
}

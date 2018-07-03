import QtQuick 2.0
import "util.js" as Util

// Calculations for "unlimited scrolling" where where we are is based
// on where the user moved an item.
QtObject {
    property int itemSize
    property int scrollbarSize
    property int currentPosition

    readonly property int initialIndex: -Math.ceil(currentPosition / itemSize)
    readonly property int initialPosition: Util.negmod(currentPosition, itemSize)
    readonly property int itemsToRender: (
        scrollbarSize / itemSize +
            2 /*one for before, one for after*/
    )
}

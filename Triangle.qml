import QtQuick 2.0

Canvas {
    property int pointTo: Qt.DownArrow;
    property color color: "black"

    readonly property var _data: {
        /*Qt::UpArrow*/    1: function (w, h) { return [[0, h], [w, h], [w/2, 0]] },
        /*Qt::DownArrow*/  2: function (w, h) { return [[0, 0], [w, 0], [w/2, h]] },
        /*Qt::LeftArrow*/  3: function (w, h) { return [[w, 0], [w, h], [0, h/2]] },
        /*Qt::RightArrow*/ 4: function (w, h) { return [[0, 0], [0, h], [w, h/2]] }
    }

    onPaint: {
        // TODO Do we need to trigger repaint somehow if any of the inputs changed?

        var mine = _data[pointTo](width, height)

        var ctx = getContext("2d")
        ctx.fillStyle = color
        ctx.beginPath()

        // The base
        ctx.moveTo.apply(ctx, mine[0])
        ctx.lineTo.apply(ctx, mine[1])

        // The point
        ctx.lineTo.apply(ctx, mine[2])

        ctx.fill()
    }
}

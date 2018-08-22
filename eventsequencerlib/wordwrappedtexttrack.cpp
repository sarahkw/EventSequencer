#include "wordwrappedtexttrack.h"

#include <QTextLayout>

int WordWrappedTextTrack::width() const
{
    return width_;
}

void WordWrappedTextTrack::setWidth(int width)
{
    if (width_ != width) {
        width_ = width;
        emit widthChanged();

        relayout();
    }
}

QString WordWrappedTextTrack::text() const
{
    return text_;
}

void WordWrappedTextTrack::setText(const QString &text)
{
    if (text_ != text) {
        text_ = text;
        emit textChanged();

        relayout();
    }
}

QFont WordWrappedTextTrack::font() const
{
    return font_;
}

void WordWrappedTextTrack::setFont(const QFont &font)
{
    if (font_ != font) {
        font_ = font;
        emit fontChanged();

        relayout();
    }
}

void WordWrappedTextTrack::relayout()
{
    std::vector<RowData> build;
    if (width_ > 0) {
        QString textWithoutNewLines = text_;
        textWithoutNewLines.replace(QLatin1Char('\n'), QChar::LineSeparator);

        QTextLayout textLayout(textWithoutNewLines, font_);
        QTextOption textOption;
        textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        textLayout.setTextOption(textOption);

        textLayout.beginLayout();
        while (true) {
            QTextLine line = textLayout.createLine();
            if (!line.isValid()) break;
            line.setLineWidth(width_);
        }
        textLayout.endLayout();

        for (int i = 0; i < textLayout.lineCount(); ++i) {
            QTextLine line = textLayout.lineAt(i);
            QString thing = text_.mid(line.textStart(), line.textLength());
            build.push_back({thing, line.textStart()});
        }
    }

    // Replace rows in between
    {
        auto numRowsInCommon = qMin(build.size(), rows_.size());
        if (numRowsInCommon > 0) {
            for (unsigned i = 0; i < numRowsInCommon; ++i) {
                rows_[i] = build[i];
            }
            emit dataChanged(createIndex(0, 0),createIndex(int(numRowsInCommon - 1), 0));
        }
    }

    // Add new rows
    if (build.size() > rows_.size()) {
        beginInsertRows(QModelIndex(), int(rows_.size()), int(build.size() - 1));
        for (auto i = rows_.size(); i < build.size(); ++i) {
            rows_.push_back(build[i]);
        }
        endInsertRows();
    } else if (rows_.size() > build.size()) {
        // Chop off the end

        // I originally wanted to chop off the end before replacing rows in between.
        // But it seems to cause QML ListView to crash.

        beginRemoveRows(QModelIndex(), int(build.size()), int(rows_.size() - 1));
        rows_.resize(build.size());
        endRemoveRows();
    }

    reposition();
}

void WordWrappedTextTrack::reposition()
{
    // TEXTOFFSET:   10   5   0
    //                  ^6           We want to find 5.

    QPoint newP(-1, -1);

    auto iter =
        std::lower_bound(rows_.rbegin(), rows_.rend(), cursorFrame_,
                         [](RowData& rd, int cf) { return cf < rd.offset; });

    if (iter != rows_.rend()) {
        if (cursorFrame_ >= iter->offset &&
            cursorFrame_ < iter->offset + iter->text.size()) {
            newP.setY(int(std::distance(iter, rows_.rend()) - 1));
            newP.setX(cursorFrame_ - iter->offset);
        }
    }
    if (cursorPosition_ != newP) {
        cursorPosition_ = newP;
        emit cursorPositionChanged();
    }
}

QPoint WordWrappedTextTrack::cursorPosition() const
{
    return cursorPosition_;
}

int WordWrappedTextTrack::calculateCursorPositionClosestTo(QPoint target)
{
    if (rows_.empty()) {
        return 0;
    }

    int row = target.y();
    if (row < 0) {
        return 0;
    }
    if (row >= int(rows_.size())) {
        return calculateMaxCursorPosition();
    }

    RowData& rd = rows_[size_t(row)];

    int column = target.x();
    if (column < 0) column = 0;
    if (rd.text.size() == 0) {
        column = 0;
    } else if (column >= rd.text.size()) {
        column = rd.text.size() - 1;
    }

    return rd.offset + column;
}

int WordWrappedTextTrack::calculateMaxCursorPosition()
{
    if (rows_.empty()) {
        return 0;
    }
    RowData& lastRow = *rows_.rbegin();
    return lastRow.offset + qMax(lastRow.text.size() - 1, 0);
}

int WordWrappedTextTrack::cursorFrame() const
{
    return cursorFrame_;
}

void WordWrappedTextTrack::setCursorFrame(int cursorFrame)
{
    if (cursorFrame_ != cursorFrame) {
        cursorFrame_ = cursorFrame;
        emit cursorFrameChanged();

        reposition();
    }
}

WordWrappedTextTrack::WordWrappedTextTrack(QObject* parent)
    : QAbstractListModel(parent), cursorPosition_(-1, -1)
{

}

int WordWrappedTextTrack::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return int(rows_.size());
}

QVariant WordWrappedTextTrack::data(const QModelIndex &index, int role) const
{
    if (index.column() == 0 && index.row() >= 0 &&
        unsigned(index.row()) < rows_.size()) {

        if (role == ModelDataRole) {
            return rows_[size_t(index.row())].text;
        } else if (role == TextOffsetRole) {
            return rows_[size_t(index.row())].offset;
        }
    }
    return {};
}

QHash<int, QByteArray> WordWrappedTextTrack::roleNames() const
{
    return {
        {ModelDataRole, "modelData"},
        {TextOffsetRole, "textOffset"}
    };
}

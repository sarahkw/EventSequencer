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
    auto numRowsInCommon = qMin(build.size(), rows_.size());
    if (numRowsInCommon > 0) {
        for (decltype(numRowsInCommon) i = 0; i < numRowsInCommon; ++i) {
            rows_[i] = build[i];
        }
        emit dataChanged(createIndex(0, 0), createIndex(numRowsInCommon - 1, 0));
    }

    // Add new rows
    if (build.size() > rows_.size()) {
        beginInsertRows(QModelIndex(), rows_.size(), build.size() - 1);
        for (auto i = rows_.size(); i < build.size(); ++i) {
            rows_.push_back(build[i]);
        }
        endInsertRows();
    } else if (rows_.size() > build.size()) {
        // Chop off the end

        // I originally wanted to chop off the end before replacing rows in between.
        // But it seems to cause QML ListView to crash.

        beginRemoveRows(QModelIndex(), build.size(), rows_.size() - 1);
        rows_.resize(build.size());
        endRemoveRows();
    }
}

WordWrappedTextTrack::WordWrappedTextTrack(QObject *parent) : QAbstractListModel(parent)
{

}

int WordWrappedTextTrack::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return rows_.size();
}

QVariant WordWrappedTextTrack::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.isValid()); // ??? Saw this in an example.
    if (index.column() == 0 &&
            index.row() >= 0 && static_cast<unsigned>(index.row()) < rows_.size()) {
        if (role == ModelDataRole) {
            return rows_[index.row()].text;
        } else if (role == TextOffsetRole) {
            return rows_[index.row()].offset;
        }
    }
    return QVariant();
}

QHash<int, QByteArray> WordWrappedTextTrack::roleNames() const
{
    return {
        {ModelDataRole, "modelData"},
        {TextOffsetRole, "textOffset"}
    };
}

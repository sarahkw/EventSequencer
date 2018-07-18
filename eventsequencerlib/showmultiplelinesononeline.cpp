#include "showmultiplelinesononeline.h"

QString ShowMultipleLinesOnOneLine::input() const
{
    return input_;
}

void ShowMultipleLinesOnOneLine::setInput(const QString &input)
{
    if (input_ != input) {
        input_ = input;
        emit inputChanged();

        output_ = input_;

        outputNewLinePositions_.clear();

        auto iter = output_.begin();
        while ((iter = std::find(iter, output_.end(), QChar('\n'))) != output_.end()) {
            *iter = QChar(' ');
            outputNewLinePositions_.append(QVariant(static_cast<int>(iter - output_.begin())));
        }

        emit outputChanged();
        emit outputNewLinePositionsChanged();
    }
}

QString ShowMultipleLinesOnOneLine::output() const
{
    return output_;
}

QVariantList ShowMultipleLinesOnOneLine::outputNewLinePositions() const
{
    return outputNewLinePositions_;
}

ShowMultipleLinesOnOneLine::ShowMultipleLinesOnOneLine(QObject *parent) : QObject(parent)
{

}

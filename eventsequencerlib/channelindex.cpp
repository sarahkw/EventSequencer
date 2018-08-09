#include "channelindex.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "eventsequencer.pb.h"

ChannelIndex::ChannelIndex()
{

}

ChannelIndex ChannelIndex::make1(int first)
{
    ChannelIndex cidx;
    cidx.first_ = first;
    return cidx;
}

ChannelIndex ChannelIndex::make2(int first, unsigned second)
{
    ChannelIndex cidx;
    cidx.first_ = first;
    cidx.hasSecond_ = true;
    cidx.second_ = second;
    return cidx;
}

ChannelIndex ChannelIndex::makeFromPathString(QString pathString, bool *success)
{
    *success = false;
    ChannelIndex cidx;

    QRegularExpression re("^(?<first>-?[0-9]+)(\\.(?<second>[0-9]+))?$");
    QRegularExpressionMatch match = re.match(pathString);
    if (match.hasMatch()) {
        *success = true;
        cidx.first_ = match.captured("first").toInt();
        if (match.capturedLength("second") > 0) {
            cidx.hasSecond_ = true;
            cidx.second_ = match.captured("second").toInt();
        }
    }
    return cidx;
}

ChannelIndex ChannelIndex::makeFromPb(const pb::ChannelIndex &pb)
{
    if (pb.idx_size() == 1) {
        return ChannelIndex::make1(pb.idx(0));
    } else if (pb.idx_size() == 2) {
        return ChannelIndex::make2(pb.idx(0), pb.idx(1));
    }

    if (pb.idx_size() != 0) {
        // 0 size shouldn't happen, but I guess it's not worth a warning since a
        // blank ChannelIndex represents the essence of 0 size.
        qWarning("ChannelIndex makeFromPb invalid idx_size %d", pb.idx_size());
    }
    return ChannelIndex();
}

void ChannelIndex::toPb(pb::ChannelIndex &pb) const
{
    pb.clear_idx();
    pb.add_idx(first_);
    if (hasSecond_) {
        pb.add_idx(second_);
    }
}

int ChannelIndex::first() const
{
    return first_;
}

bool ChannelIndex::hasSecond() const
{
    return hasSecond_;
}

unsigned ChannelIndex::second() const
{
    return second_;
}

bool ChannelIndex::operator<(const ChannelIndex &o) const
{
    if (first_ < o.first_) return true;
    else if (first_ > o.first_) return false;

    else if (hasSecond_ || o.hasSecond_) {
        if (first_ < 0) {
            if (hasSecond_ && !o.hasSecond_) return true;
            else if (!hasSecond_ && o.hasSecond_) return false;
            else return second_ > o.second_;
        } else {
            if (hasSecond_ && !o.hasSecond_) return false;
            else if (!hasSecond_ && o.hasSecond_) return true;
            else return second_ < o.second_;
        }
    }
    else return false;
}

bool ChannelIndex::operator==(const ChannelIndex &o) const
{
    return first_ == o.first_ && hasSecond_ == o.hasSecond_ && second_ == o.second_;
}

bool ChannelIndex::operator>=(const ChannelIndex &o) const
{
    return !operator<(o);
}

bool ChannelIndex::operator!=(const ChannelIndex &o) const
{
    return !operator==(o);
}

QString ChannelIndex::toDebugString() const
{
    return QString("(ChannelIndex %1%2)")
        .arg(first_)
            .arg(hasSecond_ ? QString(" %1").arg(second_) : QString());
}

QString ChannelIndex::toPathString() const
{
    if (hasSecond_) {
        return QString("%1.%2").arg(first_).arg(second_);
    } else {
        return QString::number(first_);
    }
}

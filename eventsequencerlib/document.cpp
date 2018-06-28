#include "document.h"

#include "strip.h"
#include "eventsequencer.pb.h"

#include <QDebug>

int Document::framesPerSecond() const
{
    return framesPerSecond_;
}

void Document::setFramesPerSecond(int framesPerSecond)
{
    if (framesPerSecond_ != framesPerSecond) {
        framesPerSecond_ = framesPerSecond;
        emit framesPerSecondChanged();
    }
}

Document::Document(QObject *parent) : QAbstractListModel(parent)
{

}

void Document::toPb(pb::Document &pb) const
{
    for (const Strip* s : strips_) {
        s->toPb(*pb.add_strips());
    }
    pb.set_framespersecond(framesPerSecond_);
}

int Document::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return strips_.size();
}

QVariant Document::data(const QModelIndex &index, int role) const
{
    if (role == ModelDataRole) {
        Q_ASSERT(index.row() >= 0);
        Q_ASSERT(static_cast<size_t>(index.row()) < strips_.size());

        QVariant var;
        var.setValue(strips_[index.row()]);
        return var;
    }
    return QVariant();
}

QHash<int, QByteArray> Document::roleNames() const
{
    return {{ModelDataRole, "modelData"}};
}

Strip* Document::createStrip()
{
    Strip* s = new Strip(this);

    beginInsertRows(QModelIndex(), strips_.size(), strips_.size());
    strips_.push_back(s);
    endInsertRows();

    return s;
}

void Document::deleteStrip(Strip* strip)
{
    auto found = std::find(strips_.begin(), strips_.end(), strip);
    Q_ASSERT(found != strips_.end());
    Strip* s = *found;
    auto rmIndex = found - strips_.begin();
    beginRemoveRows(QModelIndex(), rmIndex, rmIndex);
    strips_.erase(found);
    endRemoveRows();
    delete s;
}

QVariantList Document::strips()
{
    QVariantList ret;
    for (Strip* s : strips_) {
        QVariant v;
        v.setValue(s);
        ret.push_back(v);
    }
    return ret;
}

void Document::save(const QString& fileName)
{
    qInfo() << "I will eventually save to" << fileName;
    pb::Document doc;
    toPb(doc);
    doc.PrintDebugString();
    // TODO Actually save!
}

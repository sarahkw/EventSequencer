#include "document.h"

#include "strip.h"

Document::Document(QObject *parent) : QAbstractListModel(parent)
{

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

QVariant Document::createStrip()
{
    Strip* s = new Strip(this);

    beginInsertRows(QModelIndex(), strips_.size(), strips_.size());
    strips_.push_back(s);
    endInsertRows();

    QVariant var;
    var.setValue(s);
    return var;
}

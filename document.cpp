#include "document.h"

#include "strip.h"

Document::Document(QObject *parent) : QAbstractListModel(parent)
{

}

int Document::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 5;
}

QVariant Document::data(const QModelIndex &index, int role) const
{
    if (role == ModelDataRole) {
        Strip* s = new Strip;
        s->setChannel(index.row());
        s->setStartFrame(index.row());
        s->setLength(10);

        QVariant var;
        var.setValue(s);
        return var;
    }
    return QVariant();
}

QHash<int, QByteArray> Document::roleNames() const
{
    return {{ModelDataRole, "modelData"}};
}

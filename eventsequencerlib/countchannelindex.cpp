#include "countchannelindex.h"

#include "document.h"

QObject *CountChannelIndex::document() const
{
    return document_;
}

void CountChannelIndex::setDocument(QObject *document)
{
    if (document_ != document) {

        if (document_ != nullptr) {
            Document* oldCastDocument = qobject_cast<Document*>(document_);
            oldCastDocument->channelPositionManager().disconnect(this);
            oldCastDocument->disconnect(this);
        }

        Document* castDocument = qobject_cast<Document*>(document);
        document_ = castDocument;

        if (castDocument != nullptr) {
            QObject::connect(&castDocument->channelPositionManager(),
                             &VisualPositionManager::visualPositionChangedAfter,
                             this,
                             &CountChannelIndex::updateModel);
            QObject::connect(castDocument, &QObject::destroyed,
                             this, &CountChannelIndex::documentDestroyed);
        }

        emit documentChanged();
        updateModel();
    }
}

void CountChannelIndex::documentDestroyed()
{
    if (document_ != nullptr) {
        document_ = nullptr;
        emit documentChanged();
        updateModel();
    }
}

int CountChannelIndex::startAtPosition() const
{
    return startAtPosition_;
}

void CountChannelIndex::setStartAtPosition(int startAtPosition)
{
    if (startAtPosition_ != startAtPosition) {
        startAtPosition_ = startAtPosition;
        emit startAtPositionChanged();
        updateModel();
    }
}

int CountChannelIndex::count() const
{
    return count_;
}

void CountChannelIndex::setCount(int count)
{
    if (count_ != count) {
        count_ = count;
        emit countChanged();
        updateModel();
    }
}

int CountChannelIndex::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return current_.size();
}

QVariant CountChannelIndex::data(const QModelIndex &index, int role) const
{
    if (index.column() == 0 && role == ModelDataRole) {
        if (index.row() >= 0 && index.row() < current_.size()) {
            return QVariant::fromValue(current_[index.row()]);
        }
    }
    return {};
}

QHash<int, QByteArray> CountChannelIndex::roleNames() const
{
    return {{ModelDataRole, "modelData"}};
}

void CountChannelIndex::updateModel()
{
    std::vector<ChannelIndex> newItems;

    Document* doc = qobject_cast<Document*>(document_);
    if (doc != nullptr) {
        for (int i = 0; i < count(); ++i) {
            newItems.push_back(
                        doc->channelPositionManager().visualPositionToChanIdx(
                            startAtPosition() + i));
        }
    }

    int numberDataChange = std::min(newItems.size(), current_.size());
    if (numberDataChange > 0) {
        std::copy_n(newItems.begin(), numberDataChange, current_.begin());
        emit dataChanged(createIndex(0, 0), createIndex(numberDataChange - 1, 0));
    }

    if (current_.size() > newItems.size()) {
        beginRemoveRows(QModelIndex(), newItems.size(), current_.size() - 1);
        current_.resize(newItems.size());
        endRemoveRows();
    } else if (current_.size() < newItems.size()) {
        beginInsertRows(QModelIndex(), current_.size(), newItems.size() - 1);
        for (unsigned long i = current_.size(); i < newItems.size(); ++i) {
            current_.push_back(newItems[i]);
        }
        endInsertRows();
    }
}

CountChannelIndex::CountChannelIndex(QObject* parent) : QAbstractListModel(parent)
{

}

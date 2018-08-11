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

QVariantList CountChannelIndex::model() const
{
    return model_;
}

void CountChannelIndex::updateModel()
{
    Document* doc = qobject_cast<Document*>(document_);
    if (doc == nullptr) {
        return;
    }

    model_.clear();
    for (int i = 0; i < count(); ++i) {
        model_.push_back(QVariant::fromValue(
            doc->channelPositionManager().visualPositionToChanIdx(
                startAtPosition() + i)));
    }

    emit modelChanged();
}

CountChannelIndex::CountChannelIndex(QObject* parent) : QObject(parent)
{

}

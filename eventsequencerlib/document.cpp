#include "document.h"

#include "strip.h"
#include "eventsequencer.pb.h"
#include "channel/iclockrole.h"
#include "channel/channelbase.h"
#include "channel/channelfactory.h"

#include <QDebug>
#include <QFile>

/******************************************************************************/

int DocumentStripsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return d_.strips_.size();
}

QVariant DocumentStripsModel::data(const QModelIndex &index, int role) const
{
    if (role == ModelDataRole) {
        Q_ASSERT(index.row() >= 0);
        Q_ASSERT(static_cast<size_t>(index.row()) < d_.strips_.size());

        QVariant var;
        var.setValue(d_.strips_[index.row()]);
        return var;
    }
    return QVariant();
}

QHash<int,QByteArray> DocumentStripsModel::roleNames() const
{
    return {{ModelDataRole, "modelData"}};
}

/******************************************************************************/

void DocumentChannelsModel::afterAdd(int id)
{
    beginInsertRows(QModelIndex(), displayRows_.size(), displayRows_.size());
    displayRows_.push_back(id);
    endInsertRows();
}

void DocumentChannelsModel::beforeDelete(int id)
{
    auto location = std::find(displayRows_.begin(), displayRows_.end(), id);
    if (location == displayRows_.end()) {
        qWarning() << __PRETTY_FUNCTION__ << "Doesn't exist" << id;
        return;
    }
    auto index = location - displayRows_.begin();
    beginRemoveRows(QModelIndex(), index, index);
    displayRows_.erase(location);
    endRemoveRows();
}

int DocumentChannelsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return displayRows_.size();
}

QVariant DocumentChannelsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() != 0) return QVariant();
    if (index.row() >= 0 && static_cast<unsigned>(index.row()) < displayRows_.size()) {
        if (role == ModelDataRole) {
            QVariant var;
            var.setValue(d_.channels_[displayRows_[index.row()]]);
            return var;
        } else if (role == ChannelIndexRole) {
            return displayRows_[index.row()];
        }
    }
    return QVariant();
}

QHash<int, QByteArray> DocumentChannelsModel::roleNames() const
{
    return {
        {ModelDataRole, "modelData"},
        {ChannelIndexRole, "channelIndex"}
    };
}

/******************************************************************************/

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

int Document::startFrame() const
{
    return startFrame_;
}

void Document::setStartFrame(int startFrame)
{
    if (startFrame_ != startFrame) {
        startFrame_ = startFrame;
        emit startFrameChanged();
    }
}

int Document::endFrame() const
{
    return endFrame_;
}

void Document::setEndFrame(int endFrame)
{
    if (endFrame_ != endFrame) {
        endFrame_ = endFrame;
        emit endFrameChanged();
    }
}

QVariantList Document::channelsProvidingClock() const
{
    QVariantList ret;
    for (int id : channelsProvidingClock_) {
        ret.push_back(QVariant(id));
    }
    return ret;
}

void Document::channelAfterAddOrReplace(int id, QObject *channel, AddOrReplace mode)
{
    channelWaitFor_.afterAdd(id, channel);

    // TODO If we know we're adding or replacing, we would use less resources.
    auto iter = channelsProvidingClock_.find(id);
    if (dynamic_cast<channel::IClockRole*>(channel)) {
        if (iter == channelsProvidingClock_.end()) {
            channelsProvidingClock_.insert(id);
            emit channelsProvidingClockChanged();
        }
    } else {
        // We could be replacing. This channel is no longer a clock.
        if (iter != channelsProvidingClock_.end()) {
            channelsProvidingClock_.erase(iter);
            emit channelsProvidingClockChanged();
        }
    }

    switch (mode) {
    case AddOrReplace::Add:
        channelsModel_.afterAdd(id);
        break;
    case AddOrReplace::Replace:
        channelsModel_.beforeDelete(id);
        channelsModel_.afterAdd(id);
        break;
    }
}

void Document::channelBeforeDelete(int id)
{
    channelWaitFor_.beforeDelete(id);

    auto iter = channelsProvidingClock_.find(id);
    if (iter != channelsProvidingClock_.end()) {
        channelsProvidingClock_.erase(id);
        emit channelsProvidingClockChanged();
    }

    channelsModel_.beforeDelete(id);
}

QUrl Document::currentUrl() const
{
    return currentUrl_;
}

void Document::setCurrentUrl(const QUrl &currentUrl)
{
    if (currentUrl_ != currentUrl) {
        currentUrl_ = currentUrl;
        emit currentUrlChanged();
        currentFileName_ = currentUrl.toLocalFile();
        emit currentFileNameChanged();
    }
}

Document::Document(QObject *parent)
    : QObject(parent), stripsModel_(*this), channelsModel_(*this)
{

}

void Document::toPb(pb::Document &pb) const
{
    for (const Strip* s : strips_) {
        s->toPb(*pb.add_strips());
    }
    pb.set_framespersecond(framesPerSecond_);
    pb.set_startframe(startFrame_);
    pb.set_endframe(endFrame_);

    auto* pb_channels = pb.mutable_channels();
    for (auto& mappair : channels_) {
        mappair.second->toPb((*pb_channels)[mappair.first]);
    }
}

void Document::fromPb(const pb::Document &pb)
{
    // TODO Delete all current strips first!
    // TODO We do not want to create a single strip each time so don't call createStrip(),
    //      because we don't want to spam the GUI with signals.
    for (int i = 0; i < pb.strips_size(); ++i) {
        createStrip()->fromPb(pb.strips(i));
    }

    // Calling setter to fire changed signals
    setFramesPerSecond(pb.framespersecond());
    setStartFrame(pb.startframe());
    setEndFrame(pb.endframe());

    Q_ASSERT(channels_.empty()); // Because channelAfterAddOrReplace will always say Add. Replace not implemented.

    for (auto& mappair : pb.channels()) {
        const int id = mappair.first;
        channel::ChannelBase* addme =
                channel::ChannelFactory::Create(mappair.second, this);
        if (addme != nullptr) {
            channels_[id] = addme;
            channelAfterAddOrReplace(id, addme, AddOrReplace::Add);
        }
    }
}

QAbstractListModel *Document::stripsModel()
{
    return &stripsModel_;
}

Strip* Document::createStrip()
{
    Strip* s = new Strip(this);

    stripsModel_.beginInsertRows(QModelIndex(), strips_.size(), strips_.size());
    strips_.push_back(s);
    stripsModel_.endInsertRows();

    return s;
}

void Document::deleteStrip(Strip* strip)
{
    auto found = std::find(strips_.begin(), strips_.end(), strip);
    Q_ASSERT(found != strips_.end());
    Strip* s = *found;
    auto rmIndex = found - strips_.begin();
    stripsModel_.beginRemoveRows(QModelIndex(), rmIndex, rmIndex);
    strips_.erase(found);
    stripsModel_.endRemoveRows();
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

QVariantList Document::stripsOnFrame(int frame)
{
    QVariantList ret;
    for (Strip* s : strips_) {
        if (frame >= s->startFrame() && frame < (s->startFrame() + s->length())) {
            QVariant v;
            v.setValue(s);
            ret.push_back(v);
        }
    }
    return ret;
}

void Document::reset()
{
    if (strips_.size() > 0) {
        stripsModel_.beginRemoveRows(QModelIndex(), 0, strips_.size() - 1);
        strips_.clear();
        stripsModel_.endRemoveRows();
    }

    setFramesPerSecond(30);
    setStartFrame(0);
    setEndFrame(250);

    while (!channels_.empty()) {
        auto it = channels_.begin();
        channelBeforeDelete(it->first);
        channels_.erase(it);
    }

    setCurrentUrl(QUrl());
}

void Document::save(const QUrl& url)
{
    // TODO Error handling! Need to do more than just write to stdout.

    pb::Document doc;
    toPb(doc);

    QFile file(url.toLocalFile());
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open" << url;
        return;
    }
    if (!doc.SerializeToFileDescriptor(file.handle())) {
        qWarning() << "Cannot serialize";
        return;
    }
    file.close();

    setCurrentUrl(url);
}

void Document::load(const QUrl &url)
{
    reset();

    QString tmpFileName = url.toLocalFile();
    QFile file(tmpFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open";
        return;
    }
    pb::Document doc;
    if (!doc.ParseFromFileDescriptor(file.handle())) {
        qWarning() << "Cannot parse";
        return;
    }
    file.close();
    fromPb(doc);

    setCurrentUrl(url);
}

void Document::dumpProtobuf()
{
    pb::Document doc;
    toPb(doc);
    qInfo() << doc.DebugString().data(); // .data() so qInfo doesn't quote it.
}

QAbstractListModel *Document::channelsModel()
{
    return &channelsModel_;
}

QObject *Document::createChannel(int id, channel::ChannelType::Enum type)
{
    channel::ChannelBase* chan = channel::ChannelFactory::Create(type, this);

    AddOrReplace mode = AddOrReplace::Add;

    auto old = channels_.find(id);
    if (old != channels_.end()) {
        delete old->second;
        mode = AddOrReplace::Replace;
    }

    channels_[id] = chan;
    channelAfterAddOrReplace(id, chan, mode);
    return chan;
}

void Document::deleteChannel(int id)
{
    channelBeforeDelete(id);

    auto it = channels_.find(id);
    if (it != channels_.end()) {
        QObject* o = it->second;
        channels_.erase(it);
        delete o;
    } else {
        qWarning() << "Nothing removed with id" << id;
    }
}

WaitFor *Document::waitForChannel(int id)
{
    auto it = channels_.find(id);
    if (it != channels_.end()) {
        return channelWaitFor_.waitFor(id, it->second);
    } else {
        return channelWaitFor_.waitFor(id);
    }
}

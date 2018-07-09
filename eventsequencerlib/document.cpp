#include "document.h"

#include "strip.h"
#include "eventsequencer.pb.h"
#include "channel/badclockchannel.h"
#include "channel/badjschannel.h"
#include "channel/textchannel.h"
#include "channel/iclockrole.h"
#include "channel/channelbase.h"

#include <QDebug>
#include <QFile>

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

void Document::channelAfterAddOrReplace(int id, QObject *channel)
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
}

void Document::channelBeforeDelete(int id)
{
    channelWaitFor_.beforeDelete(id);

    auto iter = channelsProvidingClock_.find(id);
    if (iter != channelsProvidingClock_.end()) {
        channelsProvidingClock_.erase(id);
        emit channelsProvidingClockChanged();
    }
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

Document::Document(QObject *parent) : QAbstractListModel(parent)
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
        QObject* c = mappair.second;
        if (qobject_cast<channel::BadClockChannel*>(c)) {
            (*pb_channels)[mappair.first].mutable_badclock();
        } else if (qobject_cast<channel::BadJsChannel*>(c)) {
            (*pb_channels)[mappair.first].mutable_badjs();
        } else if (channel::TextChannel* tc = qobject_cast<channel::TextChannel*>(c)) {
            auto x = (*pb_channels)[mappair.first].mutable_text();
            x->set_fontsize(tc->fontSize());
        } else {
            qWarning() << "Unable to serialize a channel.";
        }
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

    for (auto& mappair : pb.channels()) {
        const int id = mappair.first;
        channel::ChannelBase* addme = nullptr;

        const ::pb::ChannelData& cdata = mappair.second;
        switch (cdata.channel_case()) {
        case ::pb::ChannelData::kBadClock:
            addme = new channel::BadClockChannel(this);
            break;
        case ::pb::ChannelData::kBadJs:
            addme = new channel::BadJsChannel(this);
            break;
        case ::pb::ChannelData::kText:
        {
            auto c = new channel::TextChannel(this);
            c->setFontSize(cdata.text().fontsize());
            addme = c;
            break;
        }
        case ::pb::ChannelData::CHANNEL_NOT_SET:
        default:
            qWarning() << "Unable to load channel index" << id;
            break;
        }

        if (addme != nullptr) {
            channels_[id] = addme;
            channelAfterAddOrReplace(id, addme);
        }
    }
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
        beginRemoveRows(QModelIndex(), 0, strips_.size() - 1);
        strips_.clear();
        endRemoveRows();
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

QObject *Document::createChannel(int id, channel::ChannelType::Enum type)
{
    channel::ChannelBase* chan = nullptr;
    switch (type) {
    case channel::ChannelType::BadClock:
        chan = new channel::BadClockChannel(this);
        break;
    case channel::ChannelType::BadJs:
        chan = new channel::BadJsChannel(this);
        break;
    case channel::ChannelType::Text:
        chan = new channel::TextChannel(this);
        break;
    default:
        Q_ASSERT(false);
    }

    auto old = channels_.find(id);
    if (old != channels_.end()) {
        delete old->second;
    }

    channels_[id] = chan;
    channelAfterAddOrReplace(id, chan);
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

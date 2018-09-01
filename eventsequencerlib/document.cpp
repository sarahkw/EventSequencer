#include "document.h"

#include "strip.h"
#include "eventsequencer.pb.h"
#include "channel/iclockrole.h"
#include "channel/channelbase.h"
#include "channel/channelfactory.h"
#include "saferfilereplacement.h"
#include "audioformatholder.h"

#include <QDebug>
#include <QFile>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QDir>

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

void DocumentChannelsModel::afterAdd(ChannelIndex channelIndex)
{
    beginInsertRows(QModelIndex(), displayRows_.size(), displayRows_.size());
    displayRows_.push_back(channelIndex);
    endInsertRows();
}

void DocumentChannelsModel::beforeDelete(ChannelIndex channelIndex)
{
    // XXX O(n^2) to delete all channels. Maybe have a "clear channels" function.
    auto location = std::find(displayRows_.begin(), displayRows_.end(), channelIndex);
    if (location == displayRows_.end()) {
        qWarning() << __PRETTY_FUNCTION__ << "Doesn't exist" << channelIndex.toDebugString();
        return;
    }
    auto index = location - displayRows_.begin();
    beginRemoveRows(QModelIndex(), index, index);
    displayRows_.erase(location);
    endRemoveRows();
}

void DocumentChannelsModel::invalidateChannelPositions()
{
    if (displayRows_.size() > 0) {
        emit dataChanged(createIndex(0, 0),
                         createIndex(displayRows_.size() - 1, 0),
                         {ChannelPositionRole});
    }
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
            return QVariant::fromValue(d_.channels_[displayRows_[index.row()]]);
        } else if (role == ChannelIndexRole) {
            return QVariant::fromValue(displayRows_[index.row()]);
        } else if (role == ChannelPositionRole) {
            return QVariant::fromValue(
                        d_.channelPositionManager().chanIdxToVisualPosition(displayRows_[index.row()]));
        }
    }
    return QVariant();
}

QHash<int, QByteArray> DocumentChannelsModel::roleNames() const
{
    return {
        {ModelDataRole, "modelData"},
        {ChannelIndexRole, "channelIndex"},
        {ChannelPositionRole, "channelPosition"}
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
    for (ChannelIndex channelIndex : channelsProvidingClock_) {
        ret.push_back(QVariant::fromValue(channelIndex));
    }
    return ret;
}

channel::ChannelBase* Document::defaultProgramChannel() const
{
    auto iter = channels_.find(ChannelIndex::make1(0));
    if (iter != channels_.end() &&
            iter->second->channelType() == channel::ChannelType::DocFill) {
        Q_ASSERT(iter->second->parent() != nullptr); // No GC, please.
        return iter->second;
    }
    return nullptr;
}

bool Document::audioFormatHolderSet() const
{
    return audioFormatHolder_ != nullptr;
}

void Document::setAudioFormatHolderSet(bool audioFormatHolderSet)
{
    if ((audioFormatHolder_ != nullptr) != audioFormatHolderSet) {
        if (audioFormatHolderSet) {
            Q_ASSERT(audioFormatHolder_ == nullptr);
            audioFormatHolder_ = new AudioFormatHolder(this);
            emit audioFormatHolderChanged();
        } else {
            Q_ASSERT(audioFormatHolder_ != nullptr);
            delete audioFormatHolder_;
            audioFormatHolder_ = nullptr;
            emit audioFormatHolderChanged();
        }
    }
}

QObject *Document::audioFormatHolderQObject()
{
    return audioFormatHolder_;
}

void Document::channelAfterAddOrReplace(ChannelIndex channelIndex, QObject *channel, AddOrReplace mode)
{
    channelWaitForIndex_.afterAdd(channelIndex, channel);
    channelWaitForPosition_.afterAdd(channelPositionManager().chanIdxToVisualPosition(channelIndex),
                                     channel);

    // TODO If we know we're adding or replacing, we would use less resources.
    auto iter = channelsProvidingClock_.find(channelIndex);
    if (dynamic_cast<channel::IClockRole*>(channel)) {
        if (iter == channelsProvidingClock_.end()) {
            channelsProvidingClock_.insert(channelIndex);
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
        channelsModel_.afterAdd(channelIndex);
        break;
    case AddOrReplace::Replace:
        channelsModel_.beforeDelete(channelIndex);
        channelsModel_.afterAdd(channelIndex);
        break;
    }
}

void Document::channelBeforeDelete(ChannelIndex channelIndex, channel::ChannelBase* channel)
{
    {
        QmlImmediateDestructor immediateDestructor;
        channel->beforeDelete(&immediateDestructor);
    }

    channelWaitForIndex_.beforeDelete(channelIndex);
    channelWaitForPosition_.beforeDelete(channelPositionManager().chanIdxToVisualPosition(channelIndex));

    auto iter = channelsProvidingClock_.find(channelIndex);
    if (iter != channelsProvidingClock_.end()) {
        channelsProvidingClock_.erase(channelIndex);
        emit channelsProvidingClockChanged();
    }

    channelsModel_.beforeDelete(channelIndex);
}

// VisualPositionManager signal
void Document::visualPositionChangedAfter(ChannelIndex channelIndex, int delta)
{
    auto stripsMoved = stripsOnChannel_.stripsGreaterChannel(channelIndex);
    for (auto iter = stripsMoved.first; iter != stripsMoved.second; ++iter) {
        for (auto& sh : iter->second) {
            sh.strip->channelPositionChanged();
        }
    }

    channelWaitForPosition_.deleteAfter(
                channelPositionManager().chanIdxToVisualPosition(channelIndex));

    for (auto iter = channels_.upper_bound(channelIndex);
         iter != channels_.end();
         ++iter) {
        channelWaitForPosition_.afterAdd(
                    channelPositionManager().chanIdxToVisualPosition(iter->first),
                    iter->second);
    }

    channelsModel_.invalidateChannelPositions();
}

// VisualPositionManager signal
void Document::destroyChanIdx(ChannelIndex from, ChannelIndex toExclusive)
{
    {
        // Build a vector of stuff to delete. Otherwise we'd have to take care to
        // ensure we don't hold invalid iterators.
        std::vector<Strip*> toDelete;

        {
            auto stripsDeleted = stripsOnChannel_.stripsBetweenChannels(from, toExclusive);
            for (auto iter = stripsDeleted.first; iter != stripsDeleted.second; ++iter) {
                for (auto& sh : iter->second) {
                    toDelete.push_back(sh.strip);
                }
            }
        }

        for (Strip* s : toDelete) {
            deleteStrip(s);
        }
    }
    {
        std::vector<ChannelIndex> toDelete;

        {
            auto lo = channels_.lower_bound(from);
            auto hi = channels_.lower_bound(toExclusive);
            for (auto iter = lo; iter != hi; ++iter) {
                toDelete.push_back(iter->first);
            }
        }

        for (ChannelIndex& ci : toDelete) {
            deleteChannel(ci);
        }
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

        if (!currentFileName_.isEmpty()) {
            QFileInfo fi(currentFileName_);
            QString pathPart1 = fi.dir().path();
            QString pathPart2 = fi.completeBaseName();
            fileResourceDirectory_ = pathPart1 + "/" + pathPart2 + "_data";
            exportPathGenerator_ = [pathPart1, pathPart2](QString suffix) {
                return pathPart1 + "/" + "export-" + pathPart2 + suffix;
            };
        } else {
            fileResourceDirectory_.clear();
            exportPathGenerator_ = std::function<QString (QString suffix)>();
        }
        emit fileResourceDirectoryChanged();
    }
}

QString Document::fileResourceDirectory() const
{
    return fileResourceDirectory_;
}

std::function<QString (QString suffix)> Document::exportPathGenerator()
{
    return exportPathGenerator_;
}

QString Document::fileForkedFromChecksum() const
{
    return fileForkedFromChecksum_;
}

void Document::setFileForkedFromChecksum(const QString &fileForkedFromChecksum)
{
    if (fileForkedFromChecksum_ != fileForkedFromChecksum) {
        fileForkedFromChecksum_ = fileForkedFromChecksum;
        emit fileForkedFromChecksumChanged();
    }
}

Document::ForkEditFlag Document::fileForkEditFlag() const
{
    return fileForkEditFlag_;
}

void Document::setFileForkEditFlag(const ForkEditFlag &fileForkEditFlag)
{
    if (fileForkEditFlag_ != fileForkEditFlag) {
        fileForkEditFlag_ = fileForkEditFlag;
        emit fileForkEditFlagChanged();
    }
}

const DocumentStripsOnChannel &Document::stripsOnChannel() const
{
    return stripsOnChannel_;
}

Document::Document(QObject *parent)
    : QObject(parent), stripsModel_(*this), channelsModel_(*this)
{
    QObject::connect(this, &Document::stripAfterPlaced, &stripsOnChannel_, &DocumentStripsOnChannel::stripAfterPlaced);
    QObject::connect(this, &Document::stripBeforeDelete, &stripsOnChannel_, &DocumentStripsOnChannel::stripBeforeDelete);
    QObject::connect(this, &Document::stripMoved, &stripsOnChannel_, &DocumentStripsOnChannel::stripMoved);

    QObject::connect(&channelPositionManager_, &VisualPositionManager::visualPositionChangedAfter,  this, &Document::visualPositionChangedAfter);
    QObject::connect(&channelPositionManager_, &VisualPositionManager::destroyChanIdx,              this, &Document::destroyChanIdx);
}

Document::~Document()
{
    // Rely on reset() to delete everything.
    reset();
}

template <typename T>
void DescribeType()
{
    qInfo() << "DescribeType" << __PRETTY_FUNCTION__;
}

void Document::toPb(pb::Document &pb) const
{
    // Need to build all the main channels before we could add subchannels
    // to them.
    std::map<int, std::map<unsigned, channel::ChannelBase*>> subChannels;

    for (auto& mappair : channels_) {
        if (mappair.first.hasSecond()) {
            subChannels[mappair.first.first()][mappair.first.second()] =
                    mappair.second;
        } else {
            mappair.second->toPb((*pb.mutable_channels())[mappair.first.first()]);
        }
    }

    auto mutableSpan = [&pb](int idxFirst) {
        auto& pbChannel = (*pb.mutable_channels())[idxFirst];
        Q_ASSERT(pbChannel.has_span());
        return pbChannel.mutable_span();
    };

    for (auto& mp1 : subChannels) {
        for (auto& mp2 : mp1.second) {
            const int idxFirst = mp1.first;
            const unsigned idxSecond = mp2.first;
            const channel::ChannelBase* chan = mp2.second;

            auto& chanMap = *mutableSpan(idxFirst)->mutable_channels();
            chan->toPb(chanMap[idxSecond]);
        }
    }
    
    for (const Strip* s : strips_) {
        if (!s->channelIndex().hasSecond()) {
            s->toPb(*pb.add_strips());
        } else {
            pb::Strip* pbStrip = mutableSpan(s->channelIndex().first())->add_strips();
            s->toPb(*pbStrip);
            pbStrip->set_channel(s->channelIndex().second()); // TODO: Hack!
        }
    }
    pb.set_framespersecond(framesPerSecond_);
    pb.set_startframe(startFrame_);
    pb.set_endframe(endFrame_);

    if (audioFormatHolder_ != nullptr) {
        audioFormatHolder_->toPb(*pb.mutable_audioformat());
    } else {
        pb.clear_audioformat();
    }
}

void Document::fromPb(const pb::Document &pb)
{
    // Create channels before strips, because Span channels define the structure.

    Q_ASSERT(channels_.empty()); // Because channelAfterAddOrReplace will always say Add. Replace not implemented.

    const auto addPbChannel = [this](
        ChannelIndex channelIndex,
        const pb::ChannelData& pb) -> channel::ChannelBase* {

        // TODO Refactor this to share more code with createChannel() because
        //      it's mostly doing the same.

        if (!channelPositionManager_.chanIdxIsValid(channelIndex)) {
            // Write a warning because this really shouldn't happen.
            qWarning() << "Cannot create channel on invalid index"
                       << channelIndex.toDebugString();
            return nullptr;
        }

        channel::ChannelBase* addme =
                channel::ChannelFactory::Create(pb, channelIndex, *this, this);
        if (addme != nullptr) {
            channels_[channelIndex] = addme;
            channelAfterAddOrReplace(channelIndex, addme, AddOrReplace::Add);
        }
        return addme;
    };

    for (auto& mp1 : pb.channels()) {
        const int topLevelIndex = mp1.first;
        const pb::ChannelData& topLevelPb = mp1.second;
        auto* addedChannel = addPbChannel(ChannelIndex::make1(topLevelIndex), topLevelPb);

        // Only if adding the channel succeeded do we want to add its contents.
        if (addedChannel != nullptr &&
            addedChannel->channelType() == channel::ChannelType::Span &&
            topLevelPb.has_span()) {

            for (auto& mp2 : topLevelPb.span().channels()) {
                const int childLevelIndex = mp2.first;
                const pb::ChannelData& childLevelPb = mp2.second;

                addPbChannel(ChannelIndex::make2(topLevelIndex, childLevelIndex),
                             childLevelPb);
            }
            for (int i = 0; i < topLevelPb.span().strips_size(); ++i) {
                const pb::Strip& pbStrip = topLevelPb.span().strips(i);
                Strip* s = createStrip();
                s->fromPb(pbStrip);
                // TODO Fix this ugly hack!
                s->setChannelIndex(ChannelIndex::make2(topLevelIndex,
                                                       pbStrip.channel()));
            }
        }
    }

    for (int i = 0; i < pb.strips_size(); ++i) {
        createStrip()->fromPb(pb.strips(i));
    }

    // Calling setter to fire changed signals
    setFramesPerSecond(pb.framespersecond());
    setStartFrame(pb.startframe());
    setEndFrame(pb.endframe());

    if (pb.has_audioformat()) {
        setAudioFormatHolderSet(true);
        Q_ASSERT(audioFormatHolder_ != nullptr);
        audioFormatHolder_->fromPb(pb.audioformat());
    } else {
        setAudioFormatHolderSet(false);
    }
}

QAbstractListModel *Document::stripsModel()
{
    return &stripsModel_;
}

Strip* Document::createStrip()
{
    // WARNING Strip must always have a parent, or else it'll be
    //         garbage collected.
    
    Strip* s = new Strip(*this, this);

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

    emit stripBeforeDelete(s);
    delete s;
}

QVariantList Document::stripsForQml() const
{
    QVariantList ret;
    for (Strip* s : strips_) {
        QVariant v;
        v.setValue(s);
        ret.push_back(v);
    }
    return ret;
}

QVariantList Document::stripsOnFrame(int frame) const
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

const std::vector<Strip *> &Document::strips() const
{
    return strips_;
}

void Document::reset()
{
    if (strips_.size() > 0) {
        std::vector<Strip*> deleteStrips;

        stripsModel_.beginRemoveRows(QModelIndex(), 0, strips_.size() - 1);
        strips_.swap(deleteStrips);
        stripsModel_.endRemoveRows();

        for (auto s : deleteStrips) {
            emit stripBeforeDelete(s);
            delete s;
        }
    }

    setFramesPerSecond(30);
    setStartFrame(0);
    setEndFrame(250);

    setAudioFormatHolderSet(false);

    while (!channels_.empty()) {
        auto it = channels_.begin();
        channel::ChannelBase* chan = it->second;
        channelBeforeDelete(it->first, chan);
        channels_.erase(it);
        delete chan;
    }

    setCurrentUrl(QUrl());
    setFileForkedFromChecksum(QString());
    setFileForkEditFlag(ForkEditFlag::None);
}

namespace {
struct FileHeader {
    static constexpr char MAGIC[] = "EVSEQ";
    static constexpr unsigned MAGIC_LEN = 5;
    static constexpr unsigned VERSION_LEN = 1;
    static constexpr unsigned HEADER_LEN = MAGIC_LEN + VERSION_LEN;

    unsigned char version_ = 0;

    bool readFromFile(QIODevice& dev)
    {
        QByteArray data = dev.read(HEADER_LEN);
        if (data.size() != HEADER_LEN) return false;
        if (!data.startsWith(MAGIC)) return false;
        version_ = static_cast<unsigned char>(data.at(MAGIC_LEN));
        return true;
    }
    bool writeToFile(QIODevice& dev)
    {
        return dev.write(MAGIC, MAGIC_LEN) == MAGIC_LEN &&
               dev.write(reinterpret_cast<const char*>(&version_), VERSION_LEN) == VERSION_LEN;
    }
};
constexpr char FileHeader::MAGIC[];
}

QVariantList Document::save(const QUrl& url)
{
    // TODO Error handling! Need to do more than just write to stdout.

    pb::File pbf;
    toPb(*pbf.mutable_document());

    {
        QByteArray serializedDocument(pbf.document().ByteSize(), Qt::Initialization::Uninitialized);
        pbf.document().SerializeWithCachedSizesToArray(reinterpret_cast<::google::protobuf::uint8*>(serializedDocument.data()));
        QCryptographicHash hasher(QCryptographicHash::Sha1);
        hasher.addData(serializedDocument);
        pbf.set_checksum(hasher.result().toHex().toStdString());
    }
    switch (fileForkEditFlag()) {
    case ForkEditFlag::None:
    case ForkEditFlag::Custom:
        break;
    case ForkEditFlag::Set:
        setFileForkedFromChecksum(QString::fromStdString(pbf.checksum()));
        break;
    case ForkEditFlag::Clear:
        setFileForkedFromChecksum(QString());
        break;
    }
    pbf.set_forkedfromchecksum(fileForkedFromChecksum_.toStdString());

    SaferFileReplacement sfr(url.toLocalFile());
    sfr.begin();

    QFile& file = sfr.file();

    if (!file.open(QIODevice::WriteOnly)) {
        return {false,
                QString("Cannot open file: %1: %2")
                    .arg(file.fileName())
                    .arg(file.errorString())};
    }
    FileHeader fh;
    fh.version_ = 1;
    if (!(fh.writeToFile(file) && file.flush())) {
        return {false, "Cannot write file header"};
    }
    if (!pbf.SerializeToFileDescriptor(file.handle())) {
        return {false, "Cannot serialize document to file"};
    }
    // flush not needed again because SerializeToFileDescriptor writes directly to fd.
    file.close();

    if (!sfr.commit()) {
        return {false, "Error replacing old file with new file"};
    }

    if (sfr.backupResult() == sfr.BackupResult::Failure) {
        return {false, "Failure making backup (Save was OK)"};
    }

    setFileForkEditFlag(ForkEditFlag::None);
    setCurrentUrl(url);
    return {true};
}

QVariantList Document::load(const QUrl &url)
{
    reset();

    QString tmpFileName = url.toLocalFile();
    QFile file(tmpFileName);
    // TODO Apparantly Unbuffered doesn't work for QFile on Windows. I'm not sure
    //      what the details are, but I'm guessing it's not relevant because
    //      all we need is for Qt's QFile to not eat the protobuf data before
    //      sending the file handle to protobuf.
    if (!file.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        return {false,
                QString("Cannot open file: %1: %2")
                    .arg(tmpFileName)
                    .arg(file.errorString())};
    }
    pb::File pbf;
    FileHeader fh;
    if (!fh.readFromFile(file)) {
        return {false, "Invalid file header"};
    }
    if (fh.version_ != 1) {
        return {false, "File was created from unsupported version"};
    }
    if (!pbf.ParseFromFileDescriptor(file.handle())) {
        return {false, "Cannot decode file"};
    }

    file.close();

    setFileForkedFromChecksum(QString::fromStdString(pbf.forkedfromchecksum()));

    fromPb(pbf.document());

    setCurrentUrl(url);
    return {true};
}

QVariantList Document::loadFilePath(const QString &filePath)
{
    return load(QUrl::fromLocalFile(filePath));
}

void Document::dumpProtobuf() const
{
    pb::Document doc;
    toPb(doc);
    qInfo() << doc.DebugString().data(); // .data() so qInfo doesn't quote it.
}

QAbstractListModel *Document::channelsModel()
{
    return &channelsModel_;
}

QObject *Document::createChannel(ChannelIndex channelIndex, channel::ChannelType::Enum type)
{
    if (!channelPositionManager_.chanIdxIsValid(channelIndex)) {
        // Write a warning because this really shouldn't happen.
        qWarning() << "Cannot create channel on invalid index"
                   << channelIndex.toDebugString();
        return nullptr;
    }

    channel::ChannelBase* chan = channel::ChannelFactory::Create(type, channelIndex, *this, this);

    AddOrReplace mode = AddOrReplace::Add;

    // Give people a chance to clean up reliance on the old channel.
    channel::ChannelBase* deleteMe = nullptr;

    auto old = channels_.find(channelIndex);
    if (old != channels_.end()) {
        deleteMe = old->second;
        mode = AddOrReplace::Replace;
    }

    channels_[channelIndex] = chan;
    channelAfterAddOrReplace(channelIndex, chan, mode);
    delete deleteMe;
    return chan;
}

QObject *Document::createChannelByPosition(int position, channel::ChannelType::Enum type)
{
    return createChannel(channelPositionManager().visualPositionToChanIdx(position),
                         type);
}

void Document::deleteChannel(ChannelIndex channelIndex)
{
    auto it = channels_.find(channelIndex);
    if (it != channels_.end()) {
        channel::ChannelBase* chan = it->second;
        channelBeforeDelete(channelIndex, chan);
        channels_.erase(it);
        delete chan;
    } else {
        qWarning() << "Nothing removed with id" << channelIndex.toDebugString();
    }
}

void Document::deleteChannelByPosition(int position)
{
    deleteChannel(channelPositionManager().visualPositionToChanIdx(position));
}

WaitFor *Document::waitForChannelIndex(ChannelIndex channelIndex)
{
    auto it = channels_.find(channelIndex);
    if (it != channels_.end()) {
        return channelWaitForIndex_.waitFor(channelIndex, it->second);
    } else {
        return channelWaitForIndex_.waitFor(channelIndex);
    }
}

WaitFor *Document::waitForChannelPosition(int channelPosition)
{
    auto it = channels_.find(channelPositionManager().visualPositionToChanIdx(channelPosition));
    if (it != channels_.end()) {
        return channelWaitForPosition_.waitFor(channelPosition, it->second);
    } else {
        return channelWaitForPosition_.waitFor(channelPosition);
    }
}

const VisualPositionManager &Document::channelPositionManager() const
{
    return channelPositionManager_;
}

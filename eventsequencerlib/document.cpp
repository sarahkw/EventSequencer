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

void DocumentChannelsModel::afterAdd(int id)
{
    beginInsertRows(QModelIndex(), displayRows_.size(), displayRows_.size());
    displayRows_.push_back(id);
    endInsertRows();
}

void DocumentChannelsModel::beforeDelete(int id)
{
    // XXX O(n^2) to delete all channels. Maybe have a "clear channels" function.
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

        if (!currentFileName_.isEmpty()) {
            QFileInfo fi(currentFileName_);
            fileResourceDirectory_ = fi.dir().path() + "/" + fi.completeBaseName() + "_data";
        } else {
            fileResourceDirectory_.clear();
        }
        emit fileResourceDirectoryChanged();
    }
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
}

void Document::toPb(pb::Document &pb) const
{
    for (const Strip* s : strips_) {
        s->toPb(*pb.add_strips());
    }
    pb.set_framespersecond(framesPerSecond_);
    pb.set_startframe(startFrame_);
    pb.set_endframe(endFrame_);

    if (audioFormatHolder_ != nullptr) {
        audioFormatHolder_->toPb(*pb.mutable_audioformat());
    } else {
        pb.clear_audioformat();
    }

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

    if (pb.has_audioformat()) {
        setAudioFormatHolderSet(true);
        Q_ASSERT(audioFormatHolder_ != nullptr);
        audioFormatHolder_->fromPb(pb.audioformat());
    } else {
        setAudioFormatHolderSet(false);
    }

    Q_ASSERT(channels_.empty()); // Because channelAfterAddOrReplace will always say Add. Replace not implemented.

    for (auto& mappair : pb.channels()) {
        const int id = mappair.first;
        channel::ChannelBase* addme =
                channel::ChannelFactory::Create(mappair.second, id, *this, this);
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
        channelBeforeDelete(it->first);
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
    static constexpr int MAGIC_LEN = 5;
    static constexpr int VERSION_LEN = 1;
    static constexpr int HEADER_LEN = MAGIC_LEN + VERSION_LEN;

    unsigned char version_ = 0;

    bool readFromFile(QIODevice& dev)
    {
        QByteArray data = dev.read(HEADER_LEN);
        if (data.size() != HEADER_LEN) return false;
        if (!data.startsWith(MAGIC)) return false;
        version_ = data.at(MAGIC_LEN);
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

void Document::save(const QUrl& url)
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
        qWarning() << "Cannot open" << file.fileName();
        return;
    }
    FileHeader fh;
    fh.version_ = 1;
    if (!(fh.writeToFile(file) && file.flush())) {
        qWarning() << "Cannot write file header";
        return;
    }
    if (!pbf.SerializeToFileDescriptor(file.handle())) {
        qWarning() << "Cannot serialize";
        return;
    }
    // flush not needed again because SerializeToFileDescriptor writes directly to fd.
    file.close();

    if (!sfr.commit()) {
        qWarning() << "Error finalizing file.";
        return;
    }

    if (sfr.backupResult() == sfr.BackupResult::Failure) {
        qWarning() << "Failure making backup (Save was OK)";
    }

    setFileForkEditFlag(ForkEditFlag::None);
    setCurrentUrl(url);
}

void Document::load(const QUrl &url)
{
    reset();

    QString tmpFileName = url.toLocalFile();
    QFile file(tmpFileName);
    // TODO Apparantly Unbuffered doesn't work for QFile on Windows. This will likely prevent
    //      file open from working on Win!
    if (!file.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        qWarning() << "Cannot open";
        return;
    }
    pb::File pbf;
    FileHeader fh;
    if (!fh.readFromFile(file)) {
        qWarning() << "Invalid file";
        return;
    }
    if (fh.version_ != 1) {
        qWarning() << "Unknown version" << fh.version_;
        return;
    }
    if (!pbf.ParseFromFileDescriptor(file.handle())) {
        qWarning() << "Cannot parse";
        return;
    }

    file.close();

    setFileForkedFromChecksum(QString::fromStdString(pbf.forkedfromchecksum()));

    fromPb(pbf.document());

    setCurrentUrl(url);
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

QObject *Document::createChannel(int id, channel::ChannelType::Enum type)
{
    channel::ChannelBase* chan = channel::ChannelFactory::Create(type, id, *this, this);

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

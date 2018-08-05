#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "channel/channeltype.h"
#include "waitforhost.h"
#include "documentstripsonchannel.h"
#include "channelindex.h"

#include <QAbstractListModel>
#include <QUrl>

#include <set>

namespace pb {
class Document;
}

namespace channel {
class ChannelBase;
}

class Strip;
class Document;
class AudioFormatHolder;

class DocumentStripsModel : public QAbstractListModel
{
    Q_OBJECT
    friend class Document;
    enum CustomRoles {
        ModelDataRole = Qt::UserRole + 1
    };
    Document& d_;
    DocumentStripsModel(Document& d) : d_(d) { }
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int,QByteArray> roleNames() const override;
};

class DocumentChannelsModel : public QAbstractListModel
{
    Q_OBJECT
    friend class Document;
    Document& d_;
    DocumentChannelsModel(Document& d) : d_(d) { }

    enum CustomRoles {
        ModelDataRole = Qt::UserRole + 1,
        ChannelIndexRole
    };

    std::vector<ChannelIndex> displayRows_;

    void afterAdd(ChannelIndex channelIndex);

    void beforeDelete(ChannelIndex channelIndex);

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int,QByteArray> roleNames() const override;
};

class Document : public QObject
{
    Q_OBJECT

    friend class DocumentStripsModel;
    DocumentStripsModel stripsModel_;
    Q_PROPERTY(QAbstractListModel* stripsModel READ stripsModel CONSTANT)

    friend class DocumentChannelsModel;
    DocumentChannelsModel channelsModel_;
    Q_PROPERTY(QAbstractItemModel* channelsModel READ channelsModel CONSTANT)

    std::vector<Strip*> strips_;

    int framesPerSecond_ = 30;
    int startFrame_ = 0;
    int endFrame_ = 250;

    Q_PROPERTY(int framesPerSecond READ framesPerSecond WRITE setFramesPerSecond NOTIFY framesPerSecondChanged)
    Q_PROPERTY(int startFrame READ startFrame WRITE setStartFrame NOTIFY startFrameChanged)
    Q_PROPERTY(int endFrame READ endFrame WRITE setEndFrame NOTIFY endFrameChanged)

    std::map<ChannelIndex, channel::ChannelBase*> channels_;
    WaitForHost<ChannelIndex> channelWaitFor_;
    std::set<ChannelIndex> channelsProvidingClock_;

    Q_PROPERTY(QVariantList channelsProvidingClock
               READ channelsProvidingClock
               NOTIFY channelsProvidingClockChanged)

    AudioFormatHolder* audioFormatHolder_ = nullptr;
    Q_PROPERTY(bool audioFormatHolderSet READ audioFormatHolderSet WRITE setAudioFormatHolderSet NOTIFY audioFormatHolderChanged)
    Q_PROPERTY(QObject* audioFormatHolder READ audioFormatHolderQObject                          NOTIFY audioFormatHolderChanged)

    QUrl currentUrl_;
    QString currentFileName_;
    QString fileResourceDirectory_;

    Q_PROPERTY(QUrl currentUrl READ currentUrl WRITE setCurrentUrl NOTIFY currentUrlChanged)
    Q_PROPERTY(QString currentFileName MEMBER currentFileName_ NOTIFY currentFileNameChanged)
    Q_PROPERTY(QString fileResourceDirectory MEMBER fileResourceDirectory_ NOTIFY fileResourceDirectoryChanged)

    QString fileForkedFromChecksum_;
public:
    enum class ForkEditFlag {
        None,
        Set,
        Clear,
        Custom
    };
    Q_ENUMS(ForkEditFlag)
private:
    ForkEditFlag fileForkEditFlag_ = ForkEditFlag::None;

    Q_PROPERTY(QString fileForkedFromChecksum READ fileForkedFromChecksum WRITE setFileForkedFromChecksum NOTIFY fileForkedFromChecksumChanged)
    Q_PROPERTY(ForkEditFlag fileForkEditFlag READ fileForkEditFlag WRITE setFileForkEditFlag NOTIFY fileForkEditFlagChanged)

    DocumentStripsOnChannel stripsOnChannel_;

public:

    explicit Document(QObject *parent = nullptr);

    void toPb(pb::Document& pb) const;
    void fromPb(const pb::Document& pb);

    QAbstractListModel* stripsModel();
    Q_INVOKABLE Strip* createStrip();
    Q_INVOKABLE void deleteStrip(Strip* strip);
    Q_INVOKABLE QVariantList stripsForQml() const;
    Q_INVOKABLE QVariantList stripsOnFrame(int frame) const;
    const std::vector<Strip*>& strips() const;

    Q_INVOKABLE void reset();
    Q_INVOKABLE void save(const QUrl& url);
    Q_INVOKABLE void load(const QUrl& url);
    Q_INVOKABLE void dumpProtobuf() const;

    QAbstractListModel* channelsModel();
    Q_INVOKABLE QObject* createChannel(ChannelIndex channelIndex, channel::ChannelType::Enum type);
    Q_INVOKABLE void deleteChannel(ChannelIndex channelIndex);
    Q_INVOKABLE WaitFor* waitForChannel(ChannelIndex channelIndex);

    int framesPerSecond() const;
    void setFramesPerSecond(int framesPerSecond);

    int startFrame() const;
    void setStartFrame(int startFrame);

    int endFrame() const;
    void setEndFrame(int endFrame);

    // QVariantList for use as QML model.
    QVariantList channelsProvidingClock() const;

    bool audioFormatHolderSet() const;
    void setAudioFormatHolderSet(bool audioFormatHolderSet);
    QObject* audioFormatHolderQObject();

    QUrl currentUrl() const;
    void setCurrentUrl(const QUrl &currentUrl);

    QString fileForkedFromChecksum() const;
    void setFileForkedFromChecksum(const QString &fileForkedFromChecksum);

    ForkEditFlag fileForkEditFlag() const;
    void setFileForkEditFlag(const ForkEditFlag &fileForkEditFlag);

    const DocumentStripsOnChannel& stripsOnChannel() const;

private:

    enum class AddOrReplace {
        Add,
        Replace
    };
    void channelAfterAddOrReplace(ChannelIndex channelIndex, QObject* channel, AddOrReplace mode);
    void channelBeforeDelete(ChannelIndex channelIndex);

signals:

    void framesPerSecondChanged();
    void startFrameChanged();
    void endFrameChanged();

    void channelsProvidingClockChanged();

    void audioFormatHolderChanged();

    void currentUrlChanged();
    void currentFileNameChanged();
    void fileResourceDirectoryChanged();

    void fileForkedFromChecksumChanged();
    void fileForkEditFlagChanged();

    void stripAfterPlaced(Strip* strip);
    void stripBeforeDelete(Strip* strip);
    void stripMoved(Strip* strip,
                    ChannelIndex previousChannelIndex,
                    int previousStartFrame,
                    int previousLength);

public slots:
};

#endif // DOCUMENT_H

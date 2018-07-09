#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "channel/channeltype.h"
#include "waitforhost.h"

#include <QAbstractListModel>
#include <QUrl>

#include <set>

namespace pb {
class Document;
}

class Strip;

class Document : public QAbstractListModel
{
    Q_OBJECT

    enum CustomRoles {
        ModelDataRole = Qt::UserRole + 1
    };

    std::vector<Strip*> strips_;

    int framesPerSecond_ = 30;
    int startFrame_ = 0;
    int endFrame_ = 250;

    Q_PROPERTY(int framesPerSecond READ framesPerSecond WRITE setFramesPerSecond NOTIFY framesPerSecondChanged)
    Q_PROPERTY(int startFrame READ startFrame WRITE setStartFrame NOTIFY startFrameChanged)
    Q_PROPERTY(int endFrame READ endFrame WRITE setEndFrame NOTIFY endFrameChanged)

    std::map<int, QObject*> channels_;
    WaitForHost<int> channelWaitFor_;
    std::set<int> channelsProvidingClock_;

    Q_PROPERTY(QVariantList channelsProvidingClock
               READ channelsProvidingClock
               NOTIFY channelsProvidingClockChanged)

    QUrl currentUrl_;
    QString currentFileName_;

    Q_PROPERTY(QUrl currentUrl READ currentUrl WRITE setCurrentUrl NOTIFY currentUrlChanged)
    Q_PROPERTY(QString currentFileName MEMBER currentFileName_ NOTIFY currentFileNameChanged)

public:

    explicit Document(QObject *parent = nullptr);

    void toPb(pb::Document& pb) const;
    void fromPb(const pb::Document& pb);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int,QByteArray> roleNames() const override;

    Q_INVOKABLE Strip* createStrip();
    Q_INVOKABLE void deleteStrip(Strip* strip);
    Q_INVOKABLE QVariantList strips(); // QVariantList for QML
    Q_INVOKABLE QVariantList stripsOnFrame(int frame);
    Q_INVOKABLE void reset();
    Q_INVOKABLE void save(const QUrl& url);
    Q_INVOKABLE void load(const QUrl& url);
    Q_INVOKABLE void dumpProtobuf();

    Q_INVOKABLE QObject* createChannel(int id, channel::ChannelType::Enum type);
    Q_INVOKABLE void deleteChannel(int id);
    Q_INVOKABLE WaitFor* waitForChannel(int id);

    int framesPerSecond() const;
    void setFramesPerSecond(int framesPerSecond);

    int startFrame() const;
    void setStartFrame(int startFrame);

    int endFrame() const;
    void setEndFrame(int endFrame);

    // QVariantList for use as QML model.
    QVariantList channelsProvidingClock() const;

    QUrl currentUrl() const;
    void setCurrentUrl(const QUrl &currentUrl);

private:

    // Like signals, but not.
    void channelAfterAddOrReplace(int id, QObject* channel);
    void channelBeforeDelete(int id);

signals:

    void framesPerSecondChanged();
    void startFrameChanged();
    void endFrameChanged();

    void channelsProvidingClockChanged();

    void currentUrlChanged();
    void currentFileNameChanged();

public slots:
};

#endif // DOCUMENT_H

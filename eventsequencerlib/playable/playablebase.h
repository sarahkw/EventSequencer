#ifndef PLAYABLE_PLAYABLEBASE_H
#define PLAYABLE_PLAYABLEBASE_H

#include <QObject>
#include <QUrl>

#include <functional>

class QIODevice;
class QAudioFormat;
class ConcatIODevice;
class EndianModifyingIODevice;

namespace playable {

class PlayableBase : public QObject
{
    Q_OBJECT

    QString fileResourceDirectory_;
    Q_PROPERTY(QString fileResourceDirectory READ fileResourceDirectory WRITE setFileResourceDirectory NOTIFY fileResourceDirectoryChanged)

    QString error_;

public:
    explicit PlayableBase(QObject *parent = nullptr);

    // Return nullptr on error. If nullptr is returned, errorMessage() should have
    // something.
    virtual QIODevice* createPlayableDevice(const QAudioFormat& outputFormat) = 0;

    virtual bool isFinite() const = 0;

protected:
    void setError(QString error);
public:
    QString error();

    QString fileResourceDirectory() const;
    void setFileResourceDirectory(const QString &fileResourceDirectory);

public:
    static bool buildEmiod(QString fileResourceDirectory,
                           QUrl url,
                           const QAudioFormat& outputFormat,
                           EndianModifyingIODevice** output,
                           QString* errorMessage);

protected:
    // Return false on error, setting errorMessage().
    bool appendUrlToConcatIODevice(ConcatIODevice& ciod, QUrl url,
                                   const QAudioFormat& outputFormat,
                                   std::function<void()> callback=std::function<void()>());

signals:

    void fileResourceDirectoryChanged();

public slots:
};

} // namespace playable

#endif // PLAYABLE_PLAYABLEBASE_H

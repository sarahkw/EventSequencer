#ifndef AUDIOSTRIPEXT_H
#define AUDIOSTRIPEXT_H

#include <QObject>
#include <QUrl>

namespace pb {
class Strip_Audio;
}

namespace stripext {

class AudioStripExt : public QObject
{
    Q_OBJECT

    unsigned int mediaFrameSkipBegin_;

    Q_PROPERTY(unsigned int mediaFrameSkipBegin READ mediaFrameSkipBegin WRITE setMediaFrameSkipBegin NOTIFY mediaFrameSkipBeginChanged)

public:
    explicit AudioStripExt(QObject *parent = nullptr);

    void toPb(pb::Strip_Audio& pb) const;
    void fromPb(const pb::Strip_Audio& pb);

    unsigned int mediaFrameSkipBegin() const;
    void setMediaFrameSkipBegin(unsigned int mediaFrameSkipBegin);

signals:

    void mediaFrameSkipBeginChanged();

public slots:
};

} // namespace stripext

#endif // AUDIOSTRIPEXT_H

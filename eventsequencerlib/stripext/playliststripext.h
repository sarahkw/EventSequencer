#ifndef PLAYLISTSTRIPEXT_H
#define PLAYLISTSTRIPEXT_H

#include <QObject>
#include <QUrl>

namespace pb {
class Strip_Playlist;
}

namespace stripext {

class PlaylistStripExt : public QObject
{
    Q_OBJECT

    unsigned int mediaFrameSkipBegin_;
    unsigned int mediaFrameSkipEnd_;

    Q_PROPERTY(unsigned int mediaFrameSkipBegin READ mediaFrameSkipBegin WRITE setMediaFrameSkipBegin NOTIFY mediaFrameSkipBeginChanged)
    Q_PROPERTY(unsigned int mediaFrameSkipEnd READ mediaFrameSkipEnd WRITE setMediaFrameSkipEnd NOTIFY mediaFrameSkipEndChanged)

public:
    explicit PlaylistStripExt(QObject *parent = nullptr);

    void toPb(pb::Strip_Playlist& pb) const;
    void fromPb(const pb::Strip_Playlist& pb);

    unsigned int mediaFrameSkipBegin() const;
    void setMediaFrameSkipBegin(unsigned int mediaFrameSkipBegin);

    unsigned int mediaFrameSkipEnd() const;
    void setMediaFrameSkipEnd(unsigned int mediaFrameSkipEnd);

signals:

    void mediaFrameSkipBeginChanged();
    void mediaFrameSkipEndChanged();

        
public slots:
};

} // namespace stripext

#endif // PLAYLISTSTRIPEXT_H

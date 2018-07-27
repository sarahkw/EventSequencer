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

public:
    explicit PlaylistStripExt(QObject *parent = nullptr);

    void toPb(pb::Strip_Playlist& pb) const;
    void fromPb(const pb::Strip_Playlist& pb);

signals:

public slots:
};

} // namespace stripext

#endif // PLAYLISTSTRIPEXT_H

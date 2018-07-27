#ifndef BADAUDIOSTRIPEXT_H
#define BADAUDIOSTRIPEXT_H

#include <QObject>
#include <QUrl>

namespace pb {
class Strip_BadAudio;
}

namespace stripext {

class BadAudioStripExt : public QObject
{
    Q_OBJECT

public:
    explicit BadAudioStripExt(QObject *parent = nullptr);

    void toPb(pb::Strip_BadAudio& pb) const;
    void fromPb(const pb::Strip_BadAudio& pb);

signals:

public slots:
};

} // namespace stripext

#endif // BADAUDIOSTRIPEXT_H

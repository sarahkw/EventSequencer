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

public:
    explicit AudioStripExt(QObject *parent = nullptr);

    void toPb(pb::Strip_Audio& pb) const;
    void fromPb(const pb::Strip_Audio& pb);

signals:

public slots:
};

} // namespace stripext

#endif // AUDIOSTRIPEXT_H

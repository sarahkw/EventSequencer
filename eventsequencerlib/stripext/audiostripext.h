#ifndef AUDIOSTRIPEXT_H
#define AUDIOSTRIPEXT_H

#include <QObject>

namespace pb {
class Strip_Audio;
}

namespace stripext {

class AudioStripExt : public QObject
{
    Q_OBJECT

    QString fileName_;

    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)

public:
    explicit AudioStripExt(QObject *parent = nullptr);

    void toPb(pb::Strip_Audio& pb) const;
    void fromPb(const pb::Strip_Audio& pb);

    QString fileName() const;
    void setFileName(const QString &fileName);

signals:

    void fileNameChanged();

public slots:
};

} // namespace stripext

#endif // AUDIOSTRIPEXT_H

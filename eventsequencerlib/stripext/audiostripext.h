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

    QUrl fileNameUrl_;
    QString fileName_;

    Q_PROPERTY(QUrl fileNameUrl READ fileNameUrl WRITE setFileNameUrl NOTIFY fileNameUrlChanged)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)

public:
    explicit AudioStripExt(QObject *parent = nullptr);

    void toPb(pb::Strip_Audio& pb) const;
    void fromPb(const pb::Strip_Audio& pb);

    QUrl fileNameUrl() const;
    void setFileNameUrl(const QUrl &fileNameUrl);

    QString fileName() const;
    void setFileName(const QString &fileName);

signals:

    void fileNameUrlChanged();
    void fileNameChanged();

public slots:
};

} // namespace stripext

#endif // AUDIOSTRIPEXT_H

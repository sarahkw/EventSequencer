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

    QUrl fileNameUrl_;
    QString fileName_;

    Q_PROPERTY(QUrl fileNameUrl READ fileNameUrl WRITE setFileNameUrl NOTIFY fileNameUrlChanged)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)

public:
    explicit BadAudioStripExt(QObject *parent = nullptr);

    void toPb(pb::Strip_BadAudio& pb) const;
    void fromPb(const pb::Strip_BadAudio& pb);

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

#endif // BADAUDIOSTRIPEXT_H

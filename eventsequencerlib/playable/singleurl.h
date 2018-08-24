#ifndef PLAYABLE_SINGLEURL_H
#define PLAYABLE_SINGLEURL_H

#include "playablebase.h"

namespace playable {

class SingleUrl : public PlayableBase
{
    Q_OBJECT

    QString singleUrl_;

    Q_PROPERTY(QString singleUrl READ singleUrl WRITE setSingleUrl NOTIFY singleUrlChanged)

public:
    explicit SingleUrl(QObject *parent = nullptr);

    QIODevice* createPlayableDevice(const QAudioFormat& outputFormat) override;

    QString singleUrl() const;
    void setSingleUrl(const QString &singleUrl);

signals:

    void singleUrlChanged();

public slots:
};

} // namespace playable

#endif // PLAYABLE_SINGLEURL_H

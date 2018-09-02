#ifndef PLAYABLE_TONE_H
#define PLAYABLE_TONE_H

#include "playablebase.h"

#include <QObject>

namespace playable {

class Tone : public PlayableBase
{
    Q_OBJECT

    int frequency_ = 0;
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)

public:
    explicit Tone(QObject *parent = nullptr);

    QIODevice* createPlayableDevice(const QAudioFormat& outputFormat) override;

    bool isFinite() const override;

    int frequency() const;
    void setFrequency(int frequency);

signals:

    void frequencyChanged();

};

} // namespace playable

#endif // PLAYABLE_TONE_H

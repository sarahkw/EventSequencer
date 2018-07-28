#ifndef AUDIOFORMAT_H
#define AUDIOFORMAT_H

#include <QObject>

namespace pb {
class AudioFormat;
}

class QAudioFormat;

class AudioFormat : public QObject
{
    Q_OBJECT

public:
    enum class SampleType {
        None,
        SignedInt,
        UnSignedInt,
        Float
    };
    Q_ENUM(SampleType)
    enum class Endian {
        None,
        BigEndian,
        LittleEndian
    };
    Q_ENUM(Endian)
private:

    int sampleRate_ = 0;
    int sampleSize_ = 0;
    int channelCount_ = 0;
    SampleType sampleType_ = SampleType::None;
    Endian endian_ = Endian::None;

    Q_PROPERTY(int        sampleRate   READ sampleRate   WRITE setSampleRate   NOTIFY sampleRateChanged)
    Q_PROPERTY(int        sampleSize   READ sampleSize   WRITE setSampleSize   NOTIFY sampleSizeChanged)
    Q_PROPERTY(int        channelCount READ channelCount WRITE setChannelCount NOTIFY channelCountChanged)
    Q_PROPERTY(SampleType sampleType   READ sampleType   WRITE setSampleType   NOTIFY sampleTypeChanged)
    Q_PROPERTY(Endian     endian       READ endian       WRITE setEndian       NOTIFY endianChanged)

    // Enum helpers
    Q_PROPERTY(QStringList sampleTypeModel READ sampleTypeModel CONSTANT)
    Q_PROPERTY(QStringList endianModel READ endianModel CONSTANT)
    Q_PROPERTY(int sampleTypeIndex READ sampleTypeIndex WRITE setSampleTypeIndex NOTIFY sampleTypeIndexChanged)
    Q_PROPERTY(int endianIndex READ endianIndex WRITE setEndianIndex NOTIFY endianIndexChanged)

public:
    explicit AudioFormat(QObject *parent = nullptr);

    void toPb(pb::AudioFormat& pb) const;
    void fromPb(const pb::AudioFormat& pb);

    void fromQAudioFormat(const QAudioFormat& qAudioFormat);

    int sampleRate() const;
    void setSampleRate(int sampleRate);

    int sampleSize() const;
    void setSampleSize(int sampleSize);

    int channelCount() const;
    void setChannelCount(int channelCount);

    SampleType sampleType() const;
    void setSampleType(const SampleType &sampleType);

    Endian endian() const;
    void setEndian(const Endian &endian);

    // Enum helpers
    QStringList sampleTypeModel();
    QStringList endianModel();
    int sampleTypeIndex() const;
    void setSampleTypeIndex(int sampleTypeIndex);
    int endianIndex() const;
    void setEndianIndex(int endianIndex);
    
signals:

    void sampleRateChanged();
    void sampleSizeChanged();
    void channelCountChanged();
    void sampleTypeChanged();
    void endianChanged();

    // Enum helpers
    void sampleTypeIndexChanged();
    void endianIndexChanged();

public slots:
};

#endif // AUDIOFORMAT_H

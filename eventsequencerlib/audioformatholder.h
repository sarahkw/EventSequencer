#ifndef AUDIOFORMATHOLDER_H
#define AUDIOFORMATHOLDER_H

#include <QObject>

namespace pb {
class AudioFormat;
}

class QAudioFormat;

class AudioFormat
{
    Q_GADGET
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

    Q_PROPERTY(int        sampleRate   READ sampleRate   WRITE setSampleRate  )
    Q_PROPERTY(int        sampleSize   READ sampleSize   WRITE setSampleSize  )
    Q_PROPERTY(int        channelCount READ channelCount WRITE setChannelCount)
    Q_PROPERTY(SampleType sampleType   READ sampleType   WRITE setSampleType  )
    Q_PROPERTY(Endian     endian       READ endian       WRITE setEndian      )
public:

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
};
Q_DECLARE_METATYPE(AudioFormat)

class AudioFormatHolder : public QObject
{
    Q_OBJECT

    AudioFormat audioFormat_;

    Q_PROPERTY(AudioFormat audioFormat READ audioFormat WRITE setAudioFormat NOTIFY audioFormatChanged)

    // Enum helpers
    Q_PROPERTY(QStringList sampleTypeModel READ sampleTypeModel CONSTANT)
    Q_PROPERTY(QStringList endianModel READ endianModel CONSTANT)
    Q_PROPERTY(int sampleTypeIndex READ sampleTypeIndex WRITE setSampleTypeIndex NOTIFY audioFormatChanged)
    Q_PROPERTY(int endianIndex READ endianIndex WRITE setEndianIndex NOTIFY audioFormatChanged)

public:
    explicit AudioFormatHolder(QObject *parent = nullptr);

    void toPb(pb::AudioFormat& pb) const;
    void fromPb(const pb::AudioFormat& pb);

    void fromQAudioFormat(const QAudioFormat& qAudioFormat);
    QAudioFormat toQAudioFormat() const;

    AudioFormat audioFormat() const;
    void setAudioFormat(const AudioFormat& audioFormat);

    // Enum helpers
    QStringList sampleTypeModel();
    QStringList endianModel();
    int sampleTypeIndex() const;
    void setSampleTypeIndex(int sampleTypeIndex);
    int endianIndex() const;
    void setEndianIndex(int endianIndex);
    
signals:

    void audioFormatChanged();

public slots:
};

#endif // AUDIOFORMATHOLDER_H

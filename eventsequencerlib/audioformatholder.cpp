#include "audioformatholder.h"

#include <eventsequencer.pb.h>

#include <QAudioFormat>

int AudioFormatHolder::sampleRate() const
{
    return sampleRate_;
}

void AudioFormatHolder::setSampleRate(int sampleRate)
{
    if (sampleRate_ != sampleRate) {
        sampleRate_ = sampleRate;
        emit sampleRateChanged();
    }
}

int AudioFormatHolder::sampleSize() const
{
    return sampleSize_;
}

void AudioFormatHolder::setSampleSize(int sampleSize)
{
    if (sampleSize_ != sampleSize) {
        sampleSize_ = sampleSize;
        emit sampleSizeChanged();
    }
}

int AudioFormatHolder::channelCount() const
{
    return channelCount_;
}

void AudioFormatHolder::setChannelCount(int channelCount)
{
    if (channelCount_ != channelCount) {
        channelCount_ = channelCount;
        emit channelCountChanged();
    }
}

AudioFormatHolder::SampleType AudioFormatHolder::sampleType() const
{
    return sampleType_;
}

void AudioFormatHolder::setSampleType(const SampleType &sampleType)
{
    if (sampleType_ != sampleType) {
        sampleType_ = sampleType;
        emit sampleTypeChanged();
        emit sampleTypeIndexChanged();
    }
}

AudioFormatHolder::Endian AudioFormatHolder::endian() const
{
    return endian_;
}

void AudioFormatHolder::setEndian(const Endian &endian)
{
    if (endian_ != endian) {
        endian_ = endian;
        emit endianChanged();
        emit endianIndexChanged();
    }
}

QStringList AudioFormatHolder::sampleTypeModel() /*enum helper*/
{
    const static QStringList sl{"", "Signed", "Unsigned", "Float"};
    return sl;
}

QStringList AudioFormatHolder::endianModel() /*enum helper*/
{
    const static QStringList sl{"", "Big", "Little"};
    return sl;
}

int AudioFormatHolder::sampleTypeIndex() const /*enum helper*/
{
    switch (sampleType_) {
    case SampleType::None:        return 0;
    case SampleType::SignedInt:   return 1;
    case SampleType::UnSignedInt: return 2;
    case SampleType::Float:       return 3;
    }
    return 0; // Compiler
}

void AudioFormatHolder::setSampleTypeIndex(int sampleTypeIndex) /*enum helper*/
{
    switch (sampleTypeIndex) {
    case 0: setSampleType(SampleType::None       ); break;
    case 1: setSampleType(SampleType::SignedInt  ); break;
    case 2: setSampleType(SampleType::UnSignedInt); break;
    case 3: setSampleType(SampleType::Float      ); break;
    }
}

int AudioFormatHolder::endianIndex() const /*enum helper*/
{
    switch (endian_) {
    case Endian::None:         return 0;
    case Endian::BigEndian:    return 1;
    case Endian::LittleEndian: return 2;
    }
    return 0; // Compiler
}

void AudioFormatHolder::setEndianIndex(int endianIndex) /*enum helper*/
{
    switch (endianIndex) {
    case 0: setEndian(Endian::None        ); break;
    case 1: setEndian(Endian::BigEndian   ); break;
    case 2: setEndian(Endian::LittleEndian); break;
    }
}

AudioFormatHolder::AudioFormatHolder(QObject *parent) : QObject(parent)
{

}

void AudioFormatHolder::toPb(pb::AudioFormat &pb) const
{
    pb.set_samplerate(sampleRate_);
    pb.set_samplesize(sampleSize_);
    pb.set_channelcount(channelCount_);
    switch (sampleType_) {
    case SampleType::None        : pb.set_sampletype(pb::AudioFormat_SampleType_UnsetSampleType ); break;
    case SampleType::SignedInt   : pb.set_sampletype(pb::AudioFormat_SampleType_SignedInt       ); break;
    case SampleType::UnSignedInt : pb.set_sampletype(pb::AudioFormat_SampleType_UnSignedInt     ); break;
    case SampleType::Float       : pb.set_sampletype(pb::AudioFormat_SampleType_Float           ); break;
    }
    switch (endian_) {
    case Endian::None         : pb.set_endian(pb::AudioFormat_Endian_UnsetEndian  ); break;
    case Endian::BigEndian    : pb.set_endian(pb::AudioFormat_Endian_BigEndian    ); break;
    case Endian::LittleEndian : pb.set_endian(pb::AudioFormat_Endian_LittleEndian ); break;
    }
}

void AudioFormatHolder::fromPb(const pb::AudioFormat &pb)
{
    setSampleRate(pb.samplerate());
    setSampleSize(pb.samplesize());
    setChannelCount(pb.channelcount());
    switch (pb.sampletype()) {
    case pb::AudioFormat_SampleType_UnsetSampleType : setSampleType(SampleType::None        ); break;
    case pb::AudioFormat_SampleType_SignedInt       : setSampleType(SampleType::SignedInt   ); break;
    case pb::AudioFormat_SampleType_UnSignedInt     : setSampleType(SampleType::UnSignedInt ); break;
    case pb::AudioFormat_SampleType_Float           : setSampleType(SampleType::Float       ); break;
    case pb::AudioFormat_SampleType_AudioFormat_SampleType_INT_MIN_SENTINEL_DO_NOT_USE_:
    case pb::AudioFormat_SampleType_AudioFormat_SampleType_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
    }
    switch (pb.endian()) {
    case pb::AudioFormat_Endian_UnsetEndian   : setEndian(Endian::None        ); break;
    case pb::AudioFormat_Endian_BigEndian     : setEndian(Endian::BigEndian   ); break;
    case pb::AudioFormat_Endian_LittleEndian  : setEndian(Endian::LittleEndian); break;
    case pb::AudioFormat_Endian_AudioFormat_Endian_INT_MIN_SENTINEL_DO_NOT_USE_:
    case pb::AudioFormat_Endian_AudioFormat_Endian_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
    }
}

void AudioFormatHolder::fromQAudioFormat(const QAudioFormat &qaf)
{
    setSampleRate(qaf.sampleRate());
    setSampleSize(qaf.sampleSize());
    setChannelCount(qaf.channelCount());
    switch (qaf.sampleType()) {
    case QAudioFormat::Unknown     : setSampleType(SampleType::None)        ; break;
    case QAudioFormat::SignedInt   : setSampleType(SampleType::SignedInt)   ; break;
    case QAudioFormat::UnSignedInt : setSampleType(SampleType::UnSignedInt) ; break;
    case QAudioFormat::Float       : setSampleType(SampleType::Float)       ; break;
    }
    switch (qaf.byteOrder()) {
    case QAudioFormat::BigEndian   : setEndian(Endian::BigEndian)    ; break;
    case QAudioFormat::LittleEndian: setEndian(Endian::LittleEndian) ; break;
    }
}

QAudioFormat AudioFormatHolder::toQAudioFormat() const
{
    QAudioFormat qaf;

    qaf.setSampleRate(sampleRate());
    qaf.setSampleSize(sampleSize());
    qaf.setChannelCount(channelCount());
    switch (sampleType()) {
    case SampleType::None        : qaf.setSampleType(QAudioFormat::Unknown     ) ; break;
    case SampleType::SignedInt   : qaf.setSampleType(QAudioFormat::SignedInt   ) ; break;
    case SampleType::UnSignedInt : qaf.setSampleType(QAudioFormat::UnSignedInt ) ; break;
    case SampleType::Float       : qaf.setSampleType(QAudioFormat::Float       ) ; break;
    }
    switch (endian()) {
    case Endian::BigEndian   : qaf.setByteOrder(QAudioFormat::BigEndian    ) ; break;
    case Endian::LittleEndian: qaf.setByteOrder(QAudioFormat::LittleEndian ) ; break;
    case Endian::None: // Leave the default, I guess.
        break;
    }

    qaf.setCodec("audio/pcm");

    return qaf;
}

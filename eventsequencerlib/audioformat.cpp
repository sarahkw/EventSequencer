#include "audioformat.h"

#include <eventsequencer.pb.h>

#include <QAudioFormat>

int AudioFormat::sampleRate() const
{
    return sampleRate_;
}

void AudioFormat::setSampleRate(int sampleRate)
{
    if (sampleRate_ != sampleRate) {
        sampleRate_ = sampleRate;
        emit sampleRateChanged();
    }
}

int AudioFormat::sampleSize() const
{
    return sampleSize_;
}

void AudioFormat::setSampleSize(int sampleSize)
{
    if (sampleSize_ != sampleSize) {
        sampleSize_ = sampleSize;
        emit sampleSizeChanged();
    }
}

int AudioFormat::channelCount() const
{
    return channelCount_;
}

void AudioFormat::setChannelCount(int channelCount)
{
    if (channelCount_ != channelCount) {
        channelCount_ = channelCount;
        emit channelCountChanged();
    }
}

AudioFormat::SampleType AudioFormat::sampleType() const
{
    return sampleType_;
}

void AudioFormat::setSampleType(const SampleType &sampleType)
{
    if (sampleType_ != sampleType) {
        sampleType_ = sampleType;
        emit sampleTypeChanged();
        emit sampleTypeIndexChanged();
    }
}

AudioFormat::Endian AudioFormat::endian() const
{
    return endian_;
}

void AudioFormat::setEndian(const Endian &endian)
{
    if (endian_ != endian) {
        endian_ = endian;
        emit endianChanged();
        emit endianIndexChanged();
    }
}

QStringList AudioFormat::sampleTypeModel() /*enum helper*/
{
    const static QStringList sl{"", "Signed", "Unsigned", "Float"};
    return sl;
}

QStringList AudioFormat::endianModel() /*enum helper*/
{
    const static QStringList sl{"", "Big", "Little"};
    return sl;
}

int AudioFormat::sampleTypeIndex() const /*enum helper*/
{
    switch (sampleType_) {
    case SampleType::None:        return 0;
    case SampleType::SignedInt:   return 1;
    case SampleType::UnSignedInt: return 2;
    case SampleType::Float:       return 3;
    }
    return 0; // Compiler
}

void AudioFormat::setSampleTypeIndex(int sampleTypeIndex) /*enum helper*/
{
    switch (sampleTypeIndex) {
    case 0: setSampleType(SampleType::None       ); break;
    case 1: setSampleType(SampleType::SignedInt  ); break;
    case 2: setSampleType(SampleType::UnSignedInt); break;
    case 3: setSampleType(SampleType::Float      ); break;
    }
}

int AudioFormat::endianIndex() const /*enum helper*/
{
    switch (endian_) {
    case Endian::None:         return 0;
    case Endian::BigEndian:    return 1;
    case Endian::LittleEndian: return 2;
    }
    return 0; // Compiler
}

void AudioFormat::setEndianIndex(int endianIndex) /*enum helper*/
{
    switch (endianIndex) {
    case 0: setEndian(Endian::None        ); break;
    case 1: setEndian(Endian::BigEndian   ); break;
    case 2: setEndian(Endian::LittleEndian); break;
    }
}

AudioFormat::AudioFormat(QObject *parent) : QObject(parent)
{

}

void AudioFormat::toPb(pb::AudioFormat &pb) const
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

void AudioFormat::fromPb(const pb::AudioFormat &pb)
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

void AudioFormat::fromQAudioFormat(const QAudioFormat &qaf)
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

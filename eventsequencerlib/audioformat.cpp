#include "audioformat.h"

#include <eventsequencer.pb.h>

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

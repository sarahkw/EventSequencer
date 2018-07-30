#include "audioformatholder.h"

#include <eventsequencer.pb.h>

#include <QAudioFormat>

int AudioFormat::sampleRate() const
{
    return sampleRate_;
}

void AudioFormat::setSampleRate(int sampleRate)
{
    sampleRate_ = sampleRate;
}

int AudioFormat::sampleSize() const
{
    return sampleSize_;
}

void AudioFormat::setSampleSize(int sampleSize)
{
    sampleSize_ = sampleSize;
}

int AudioFormat::channelCount() const
{
    return channelCount_;
}

void AudioFormat::setChannelCount(int channelCount)
{
    channelCount_ = channelCount;
}

AudioFormat::SampleType AudioFormat::sampleType() const
{
    return sampleType_;
}

void AudioFormat::setSampleType(const SampleType &sampleType)
{
    sampleType_ = sampleType;
}

AudioFormat::Endian AudioFormat::endian() const
{
    return endian_;
}

void AudioFormat::setEndian(const Endian &endian)
{
    endian_ = endian;
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
    switch (audioFormat_.sampleType()) {
    case AudioFormat::SampleType::None:        return 0;
    case AudioFormat::SampleType::SignedInt:   return 1;
    case AudioFormat::SampleType::UnSignedInt: return 2;
    case AudioFormat::SampleType::Float:       return 3;
    }
    return 0; // Compiler
}

void AudioFormatHolder::setSampleTypeIndex(int sampleTypeIndex) /*enum helper*/
{
    switch (sampleTypeIndex) {
    case 0: audioFormat_.setSampleType(AudioFormat::SampleType::None       ); break;
    case 1: audioFormat_.setSampleType(AudioFormat::SampleType::SignedInt  ); break;
    case 2: audioFormat_.setSampleType(AudioFormat::SampleType::UnSignedInt); break;
    case 3: audioFormat_.setSampleType(AudioFormat::SampleType::Float      ); break;
    }
    emit audioFormatChanged();
}

int AudioFormatHolder::endianIndex() const /*enum helper*/
{
    switch (audioFormat_.endian()) {
    case AudioFormat::Endian::None:         return 0;
    case AudioFormat::Endian::BigEndian:    return 1;
    case AudioFormat::Endian::LittleEndian: return 2;
    }
    return 0; // Compiler
}

void AudioFormatHolder::setEndianIndex(int endianIndex) /*enum helper*/
{
    switch (endianIndex) {
    case 0: audioFormat_.setEndian(AudioFormat::Endian::None        ); break;
    case 1: audioFormat_.setEndian(AudioFormat::Endian::BigEndian   ); break;
    case 2: audioFormat_.setEndian(AudioFormat::Endian::LittleEndian); break;
    }
    emit audioFormatChanged();
}

AudioFormatHolder::AudioFormatHolder(QObject *parent) : QObject(parent)
{

}

void AudioFormatHolder::toPb(pb::AudioFormat &pb) const
{
    pb.set_samplerate(audioFormat_.sampleRate());
    pb.set_samplesize(audioFormat_.sampleSize());
    pb.set_channelcount(audioFormat_.channelCount());
    switch (audioFormat_.sampleType()) {
    case AudioFormat::SampleType::None        : pb.set_sampletype(pb::AudioFormat_SampleType_UnsetSampleType ); break;
    case AudioFormat::SampleType::SignedInt   : pb.set_sampletype(pb::AudioFormat_SampleType_SignedInt       ); break;
    case AudioFormat::SampleType::UnSignedInt : pb.set_sampletype(pb::AudioFormat_SampleType_UnSignedInt     ); break;
    case AudioFormat::SampleType::Float       : pb.set_sampletype(pb::AudioFormat_SampleType_Float           ); break;
    }
    switch (audioFormat_.endian()) {
    case AudioFormat::Endian::None         : pb.set_endian(pb::AudioFormat_Endian_UnsetEndian  ); break;
    case AudioFormat::Endian::BigEndian    : pb.set_endian(pb::AudioFormat_Endian_BigEndian    ); break;
    case AudioFormat::Endian::LittleEndian : pb.set_endian(pb::AudioFormat_Endian_LittleEndian ); break;
    }
}

void AudioFormatHolder::fromPb(const pb::AudioFormat &pb)
{
    audioFormat_.setSampleRate(pb.samplerate());
    audioFormat_.setSampleSize(pb.samplesize());
    audioFormat_.setChannelCount(pb.channelcount());
    switch (pb.sampletype()) {
    case pb::AudioFormat_SampleType_UnsetSampleType : audioFormat_.setSampleType(AudioFormat::SampleType::None        ); break;
    case pb::AudioFormat_SampleType_SignedInt       : audioFormat_.setSampleType(AudioFormat::SampleType::SignedInt   ); break;
    case pb::AudioFormat_SampleType_UnSignedInt     : audioFormat_.setSampleType(AudioFormat::SampleType::UnSignedInt ); break;
    case pb::AudioFormat_SampleType_Float           : audioFormat_.setSampleType(AudioFormat::SampleType::Float       ); break;
    case pb::AudioFormat_SampleType_AudioFormat_SampleType_INT_MIN_SENTINEL_DO_NOT_USE_:
    case pb::AudioFormat_SampleType_AudioFormat_SampleType_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
    }
    switch (pb.endian()) {
    case pb::AudioFormat_Endian_UnsetEndian   : audioFormat_.setEndian(AudioFormat::Endian::None        ); break;
    case pb::AudioFormat_Endian_BigEndian     : audioFormat_.setEndian(AudioFormat::Endian::BigEndian   ); break;
    case pb::AudioFormat_Endian_LittleEndian  : audioFormat_.setEndian(AudioFormat::Endian::LittleEndian); break;
    case pb::AudioFormat_Endian_AudioFormat_Endian_INT_MIN_SENTINEL_DO_NOT_USE_:
    case pb::AudioFormat_Endian_AudioFormat_Endian_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
    }

    emit audioFormatChanged();
}

void AudioFormatHolder::fromQAudioFormat(const QAudioFormat &qaf)
{
    audioFormat_.setSampleRate(qaf.sampleRate());
    audioFormat_.setSampleSize(qaf.sampleSize());
    audioFormat_.setChannelCount(qaf.channelCount());
    switch (qaf.sampleType()) {
    case QAudioFormat::Unknown     : audioFormat_.setSampleType(AudioFormat::SampleType::None)        ; break;
    case QAudioFormat::SignedInt   : audioFormat_.setSampleType(AudioFormat::SampleType::SignedInt)   ; break;
    case QAudioFormat::UnSignedInt : audioFormat_.setSampleType(AudioFormat::SampleType::UnSignedInt) ; break;
    case QAudioFormat::Float       : audioFormat_.setSampleType(AudioFormat::SampleType::Float)       ; break;
    }
    switch (qaf.byteOrder()) {
    case QAudioFormat::BigEndian   : audioFormat_.setEndian(AudioFormat::Endian::BigEndian)    ; break;
    case QAudioFormat::LittleEndian: audioFormat_.setEndian(AudioFormat::Endian::LittleEndian) ; break;
    }

    emit audioFormatChanged();
}

QAudioFormat AudioFormatHolder::toQAudioFormat() const
{
    QAudioFormat qaf;

    qaf.setSampleRate(audioFormat_.sampleRate());
    qaf.setSampleSize(audioFormat_.sampleSize());
    qaf.setChannelCount(audioFormat_.channelCount());
    switch (audioFormat_.sampleType()) {
    case AudioFormat::SampleType::None        : qaf.setSampleType(QAudioFormat::Unknown     ) ; break;
    case AudioFormat::SampleType::SignedInt   : qaf.setSampleType(QAudioFormat::SignedInt   ) ; break;
    case AudioFormat::SampleType::UnSignedInt : qaf.setSampleType(QAudioFormat::UnSignedInt ) ; break;
    case AudioFormat::SampleType::Float       : qaf.setSampleType(QAudioFormat::Float       ) ; break;
    }
    switch (audioFormat_.endian()) {
    case AudioFormat::Endian::BigEndian   : qaf.setByteOrder(QAudioFormat::BigEndian    ) ; break;
    case AudioFormat::Endian::LittleEndian: qaf.setByteOrder(QAudioFormat::LittleEndian ) ; break;
    case AudioFormat::Endian::None: // Leave the default, I guess.
        break;
    }

    qaf.setCodec("audio/pcm");

    return qaf;
}

AudioFormat AudioFormatHolder::audioFormat() const
{
    return audioFormat_;
}

void AudioFormatHolder::setAudioFormat(const AudioFormat &audioFormat)
{
    audioFormat_ = audioFormat;
    emit audioFormatChanged();
}

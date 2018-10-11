#include "mp3encoder.h"

#include "entireunitreader.h"

#include <lame/lame.h>

#include <cmath>

#include <QThread>

Mp3Encoder::Mp3Encoder(QIODevice* source, QAudioFormat* sourceFormat,
                       QIODevice* destination, int mp3Quality)
    : source_(source),
      sourceFormat_(sourceFormat),
      destination_(destination),
      mp3Quality_(mp3Quality)
{
    lame_ = lame_init();
}

Mp3Encoder::~Mp3Encoder()
{
    if (lame_ != nullptr) {
        lame_close(lame_);
    }
}

Mp3Encoder::Result Mp3Encoder::init()
{
    if (lame_ == nullptr) {
        return {false, "Lame could not initialize"};
    }
    if (!sourceFormat_->isValid()) {
        return {false, "Internal error"};
    }
    if (sourceFormat_->channelCount() != 1 &&
            sourceFormat_->channelCount() != 2) {
        return {false, "Only mono and stereo supported"};
    }
    if (sourceFormat_->byteOrder() !=
        static_cast<QAudioFormat::Endian>(QSysInfo::ByteOrder)) {
        return {false, "Input has wrong byte order"};
    }

    lame_set_in_samplerate(lame_, sourceFormat_->sampleRate());
    lame_set_num_channels(lame_, sourceFormat_->channelCount());

    lame_set_VBR(lame_, vbr_default);
    lame_set_VBR_q(lame_, mp3Quality_);

    // We don't want an automatic id3 tag because we need to keep
    // track of the zero frame that's to be replaced with the VBR
    // info.
    lame_set_write_id3tag_automatic(lame_, 0);

    {
        int rcode = lame_init_params(lame_);
        if (rcode < 0) {
            return {false, QString("Lame could not init params: %1").arg(rcode)};
        }
    }

//    id3tag_init(lame_);
//    {
//        auto size = lame_get_id3v2_tag(lame_, nullptr, 0);
//        std::vector<unsigned char> tag(size);
//        size = lame_get_id3v2_tag(lame_, tag.data(), tag.size());
//        if (size != destination_->write(reinterpret_cast<char*>(tag.data()), size)) {
//            return {false, "Cannot write ID3 header"};
//        }
//    }

    Q_ASSERT(sourceFormat_->sampleSize() > 0);
    Q_ASSERT(sourceFormat_->channelCount() > 0);
    const auto bytesPerSample = size_t(sourceFormat_->sampleSize()) / 8;
    frameCount_ = bufferSize_ / bytesPerSample / size_t(sourceFormat_->channelCount());
    sampleCount_ = frameCount_ * size_t(sourceFormat_->channelCount());

    // "worst case" from docs. I add ceil just to be safe, since the doc isn't clear.
    mp3Buffer_.resize(size_t(std::ceil(1.25 * sampleCount_ + 7200)));

    sampleType_ = SupportedAudioFormat::classify(*sourceFormat_);
    switch (sampleType_) {
    case SupportedAudioFormat::Type::NotSupported:
        return {false, "Unsupported audio format for encoding"};
    case SupportedAudioFormat::Type::Float:
        samplesFloat_.resize(sampleCount_);
        break;
    case SupportedAudioFormat::Type::Short:
        samplesShort_.resize(sampleCount_);
        break;
    case SupportedAudioFormat::Type::Int:
        samplesInt_.resize(sampleCount_);
        break;
    }

    return {true, {}};
}

Mp3Encoder::Result Mp3Encoder::fillBuffer(size_t* framesRead)
{
    EntireUnitReader::Error error{};

    EntireUnitReader::QtReadFn readFn = [this](char* data, qint64 maxlen) -> qint64 {
        return source_->read(data, maxlen);
    };

    switch (sampleType_) {
    case SupportedAudioFormat::Type::NotSupported:
        Q_ASSERT(false);
        break;
    case SupportedAudioFormat::Type::Float:
        *framesRead = EntireUnitReader::readUnitGroups<float>(
            readFn, samplesFloat_.data(), samplesFloat_.size(), frameCount_,
            size_t(sourceFormat_->channelCount()), &error);
        break;
    case SupportedAudioFormat::Type::Short:
        *framesRead = EntireUnitReader::readUnitGroups<short>(
            readFn, samplesShort_.data(), samplesShort_.size(), frameCount_,
            size_t(sourceFormat_->channelCount()), &error);
        break;
    case SupportedAudioFormat::Type::Int:
        *framesRead = EntireUnitReader::readUnitGroups<int>(
            readFn, samplesInt_.data(), samplesInt_.size(), frameCount_,
            size_t(sourceFormat_->channelCount()), &error);
        break;
    }

    if (error & EntireUnitReader::NegativeOneError) {
        return {false, "Error occured while reading from source"};
    }
    if (error & EntireUnitReader::DiscardedDataDueToEofError) {
        qWarning("Discarded data while encoding mp3!");
    }

    return {true, {}};
}

void Mp3Encoder::encodeBufferMono(size_t framesRead, int *encodeBufferResult)
{
    switch (sampleType_) {
    case SupportedAudioFormat::Type::NotSupported:
        Q_ASSERT(false);
        break;
    case SupportedAudioFormat::Type::Float:
        *encodeBufferResult = lame_encode_buffer_ieee_float(
            lame_, samplesFloat_.data(), nullptr, int(framesRead),
            mp3Buffer_.data(), int(mp3Buffer_.size()));
        break;
    case SupportedAudioFormat::Type::Short:
        *encodeBufferResult = lame_encode_buffer(
            lame_, samplesShort_.data(), nullptr, int(framesRead),
            mp3Buffer_.data(), int(mp3Buffer_.size()));
        break;
    case SupportedAudioFormat::Type::Int:
        *encodeBufferResult = lame_encode_buffer_int(
            lame_, samplesInt_.data(), nullptr, int(framesRead),
            mp3Buffer_.data(), int(mp3Buffer_.size()));
        break;
    }
}

void Mp3Encoder::encodeBufferStereo(size_t framesRead, int *encodeBufferResult)
{
    switch (sampleType_) {
    case SupportedAudioFormat::Type::NotSupported:
        Q_ASSERT(false);
        break;
    case SupportedAudioFormat::Type::Float:
        *encodeBufferResult = lame_encode_buffer_interleaved_ieee_float(
            lame_, samplesFloat_.data(), int(framesRead), mp3Buffer_.data(),
            int(mp3Buffer_.size()));
        break;
    case SupportedAudioFormat::Type::Short:
        *encodeBufferResult = lame_encode_buffer_interleaved(
            lame_, samplesShort_.data(), int(framesRead), mp3Buffer_.data(),
            int(mp3Buffer_.size()));
        break;
    case SupportedAudioFormat::Type::Int:
        *encodeBufferResult = lame_encode_buffer_interleaved_int(
            lame_, samplesInt_.data(), int(framesRead), mp3Buffer_.data(),
            int(mp3Buffer_.size()));
        break;
    }
}

Mp3Encoder::Result Mp3Encoder::encode()
{
    for (;;) {
        size_t framesRead;
        {
            auto result = fillBuffer(&framesRead);
            if (!result.success) {
                return result;
            }
        }

        int encodeBufferResult;
        if (sourceFormat_->channelCount() == 1) {
            encodeBufferMono(framesRead, &encodeBufferResult);
        } else {
            Q_ASSERT(sourceFormat_->channelCount() == 2);
            encodeBufferStereo(framesRead, &encodeBufferResult);
        }

        if (encodeBufferResult < 0) {
            return {false, QString("Encode error: %1").arg(encodeBufferResult)};
        }

        if (encodeBufferResult !=
            destination_->write(reinterpret_cast<char*>(mp3Buffer_.data()),
                                encodeBufferResult)) {
            return {false, QString("Write error")};
        }

        if (framesRead < frameCount_) {
            break;  // EOF
        }

        if (QThread::currentThread()->isInterruptionRequested()) {
            return {false, "Encoding canceled"};
        }
    }

    {
        const int flushBufferResult =
            lame_encode_flush(lame_, mp3Buffer_.data(), int(mp3Buffer_.size()));
        if (flushBufferResult < 0) {
            return {false, QString("Encode error: %1").arg(flushBufferResult)};
        }
        if (flushBufferResult !=
            destination_->write(reinterpret_cast<char*>(mp3Buffer_.data()),
                                flushBufferResult)) {
            return {false, QString("Write Error")};
        }
    }

    // Note: If we ever start adding ID3v2 tags in the future, we'll have to
    // seek past it.
    if (!destination_->seek(0)) {
        return {false, "Error seeking for lametag"};
    }

    {
        // XXX Maybe it's not so good that we're allocating yet another buffer
        //     while we already have mp3Buffer_.
        auto size = lame_get_lametag_frame(lame_, nullptr, 0);
        std::vector<unsigned char> tag(size);
        size = lame_get_lametag_frame(lame_, tag.data(), tag.size());
        if (qint64(size) !=
            destination_->write(reinterpret_cast<char*>(tag.data()),
                                qint64(size))) {
            return {false, "Error writing lame tag"};
        }
    }

    return {true, {}};
}

Mp3Encoder::Result Mp3Encoder::initAndEncode()
{
    {
        auto result = init();
        if (!result.success) {
            return result;
        }
    }
    return encode();
}

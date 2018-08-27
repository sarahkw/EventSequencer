#include "aufileheader.h"

#include <QtEndian>

namespace {
static const quint32 AU_MAGIC = 0x2e736e64; // ".snd"
static const quint32 AU_MIN_DATA_OFFSET = 24;
static const quint32 AU_ENCODING_LINEAR_PCM_8           = 2;
static const quint32 AU_ENCODING_LINEAR_PCM_16          = 3;
static const quint32 AU_ENCODING_LINEAR_PCM_24          = 4;
static const quint32 AU_ENCODING_LINEAR_PCM_32          = 5;
static const quint32 AU_ENCODING_IEEE_FLOATING_POINT_32 = 6;
static const quint32 AU_ENCODING_IEEE_FLOATING_POINT_64 = 7;

struct AuHeader {
    quint32 magic_number = AU_MAGIC;
    quint32 data_offset = AU_MIN_DATA_OFFSET;
    quint32 data_size;
    quint32 encoding;
    quint32 sample_rate;
    quint32 channels;

    void toBigEndian() // Au in big endian.
    {
        magic_number = qToBigEndian(magic_number);
        data_offset  = qToBigEndian(data_offset );
        data_size    = qToBigEndian(data_size   );
        encoding     = qToBigEndian(encoding    );
        sample_rate  = qToBigEndian(sample_rate );
        channels     = qToBigEndian(channels    );
    }
    void fromBigEndian()
    {
        magic_number = qFromBigEndian(magic_number);
        data_offset  = qFromBigEndian(data_offset );
        data_size    = qFromBigEndian(data_size   );
        encoding     = qFromBigEndian(encoding    );
        sample_rate  = qFromBigEndian(sample_rate );
        channels     = qFromBigEndian(channels    );
    }
};

bool toAuEncoding(const QAudioFormat& input, quint32* output)
{
    if (input.sampleType() == QAudioFormat::SampleType::SignedInt) {
        if      (input.sampleSize() == 8 ) *output = AU_ENCODING_LINEAR_PCM_8;
        else if (input.sampleSize() == 16) *output = AU_ENCODING_LINEAR_PCM_16;
        else if (input.sampleSize() == 24) *output = AU_ENCODING_LINEAR_PCM_24;
        else if (input.sampleSize() == 32) *output = AU_ENCODING_LINEAR_PCM_32;
        else {
            return false;
        }
        return true;
    } else if (input.sampleType() == QAudioFormat::SampleType::Float) {
        if      (input.sampleSize() == 32) *output = AU_ENCODING_IEEE_FLOATING_POINT_32;
        else if (input.sampleSize() == 64) *output = AU_ENCODING_IEEE_FLOATING_POINT_64;
        else {
            return false;
        }
        return true;
    }
    return false;
}

void fromAuEncoding(quint32 input, QAudioFormat* output)
{
    switch (input) {
    case AU_ENCODING_LINEAR_PCM_8          : output->setSampleType(QAudioFormat::SampleType::SignedInt); output->setSampleSize(8 ); break;
    case AU_ENCODING_LINEAR_PCM_16         : output->setSampleType(QAudioFormat::SampleType::SignedInt); output->setSampleSize(16); break;
    case AU_ENCODING_LINEAR_PCM_24         : output->setSampleType(QAudioFormat::SampleType::SignedInt); output->setSampleSize(24); break;
    case AU_ENCODING_LINEAR_PCM_32         : output->setSampleType(QAudioFormat::SampleType::SignedInt); output->setSampleSize(32); break;
    case AU_ENCODING_IEEE_FLOATING_POINT_32: output->setSampleType(QAudioFormat::SampleType::Float);     output->setSampleSize(32); break;
    case AU_ENCODING_IEEE_FLOATING_POINT_64: output->setSampleType(QAudioFormat::SampleType::Float);     output->setSampleSize(64); break;
    }
}

}

bool AuFileHeader::loadFormat(const QAudioFormat &af)
{
    quint32 dummy;
    if (!toAuEncoding(af, &dummy)) {
        return false;
    }
    if (af.codec() != "audio/pcm") {
        return false;
    }

    audioFormat_ = af;
    return true;
}

namespace {
struct AnnotationHeader {
    // Baking the version number into the MAGIC string because it
    // seems like .au intends on the variable length annotation to be
    // text-y because the wikipedia article says it should end with a
    // null byte. So let's avoid binary data. Maybe other .au
    // applications can show this data to the user? I don't have time
    // to research more, I need to release.
    static constexpr char MAGIC[] = "EVRES0";
    static constexpr unsigned MAGIC_LEN = 6;
    static constexpr unsigned HEADER_LEN = MAGIC_LEN;

    // Au says the annotation must be a multiple of 8 bytes. It also must
    // be null terminated.
    struct AnnotationSizeResult {
        size_t writtenToDiskSize;
        size_t actualDataSize;
        size_t endPaddingSize;
    };
    static AnnotationSizeResult writtenSizeOfAnnotation(const std::string& annotation)
    {
        AnnotationSizeResult asr{};

        const size_t null_terminated = 1;
        asr.actualDataSize = annotation.size() + null_terminated + HEADER_LEN;

        if (asr.actualDataSize % 8 != 0) {
            asr.writtenToDiskSize = (asr.actualDataSize / 8 + 1) * 8;
        } else {
            asr.writtenToDiskSize = asr.actualDataSize;
        }

        asr.endPaddingSize = asr.writtenToDiskSize - asr.actualDataSize;
        return asr;
    }

    // annotation is a null terminated string
    bool readFromFile(QIODevice& dev, qint64 maxSize, std::string* annotation)
    {
        return false;
    }

    // annotation is a null terminated string
    static bool writeToFile(QIODevice& dev, const std::string& annotation)
    {
        Q_ASSERT(!annotation.empty());
        if (dev.write(MAGIC, MAGIC_LEN) != MAGIC_LEN) {
            return false;
        }
        if (dev.write(annotation.data(), annotation.size()) != annotation.size()) {
            return false;
        }

        std::vector<char> zeroes(writtenSizeOfAnnotation(annotation).endPaddingSize);
        if (dev.write(zeroes.data(), zeroes.size()) != zeroes.size()) {
            return false;
        }

        return true;
    }
};
constexpr char AnnotationHeader::MAGIC[];
}

bool AuFileHeader::loadFileAndSeek(QIODevice &device)
{
    AuHeader auh;
    // For files, .read() should read it all unless there's an error.
    {
        auto readBytes = device.read(reinterpret_cast<char*>(&auh), sizeof(auh));
        if (readBytes != sizeof(auh)) return false;
    }
    auh.fromBigEndian();

    if (auh.magic_number != AU_MAGIC) {
        return false;
    }
    quint32 seekTo = auh.data_offset;
    dataSize_ = auh.data_size;
    fromAuEncoding(auh.encoding, &audioFormat_);
    audioFormat_.setSampleRate(static_cast<int>(auh.sample_rate));
    audioFormat_.setChannelCount(static_cast<int>(auh.channels));
    audioFormat_.setByteOrder(QAudioFormat::BigEndian);
    audioFormat_.setCodec("audio/pcm");
    return device.seek(seekTo);
}

unsigned int AuFileHeader::dataSize()
{
    return dataSize_;
}

void AuFileHeader::setDataSize(unsigned int dataSize)
{
    dataSize_ = dataSize;
}

bool AuFileHeader::writeFile(QIODevice &device, const std::string& annotation)
{
    AuHeader auh;
    auh.data_offset += AnnotationHeader::writtenSizeOfAnnotation(annotation).writtenToDiskSize;
    auh.data_size = dataSize_;
    {
        bool success = toAuEncoding(audioFormat_, &auh.encoding);
        Q_ASSERT(success);
    }
    auh.sample_rate = static_cast<quint32>(audioFormat_.sampleRate());
    auh.channels = static_cast<quint32>(audioFormat_.channelCount());
    static_assert (sizeof(auh) == AU_MIN_DATA_OFFSET, "");

    auh.toBigEndian();
    // For files, .write() should write it all unless there's an error.
    if (sizeof(auh) != device.write(reinterpret_cast<const char*>(&auh), sizeof(auh))) {
        qWarning("AuFileHeader failed writing header");
        return false;
    }
    if (!annotation.empty()) {
        if (!AnnotationHeader::writeToFile(device, annotation)) {
            qWarning("AuFileHeader failed writing annotation");
            return false;
        }
    }

    return true;
}

const QAudioFormat &AuFileHeader::audioFormat()
{
    return audioFormat_;
}

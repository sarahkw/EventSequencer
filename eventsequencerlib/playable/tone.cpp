#include "tone.h"

#include <QAudioFormat>
#include <QIODevice>

namespace {

class LoopedBytesIODevice : public QIODevice {
    std::vector<char> source_;
    size_t position_ = 0;
public:
    LoopedBytesIODevice(std::vector<char>&& source) : source_(std::move(source)) {}

protected:
    qint64 readData(char *data, qint64 maxlen) override
    {
        if (source_.size() == 0) {
            return -1; // No infinite loop, please.
        }
        qint64 left = maxlen;
        while (left > 0) {
            auto toCopy = qMin(size_t(left), source_.size() - position_);
            memcpy(data, source_.data() + position_, toCopy);
            data += toCopy;
            left -= toCopy;
            position_ = (position_ + toCopy) % source_.size();
        }
        return maxlen;
    }
    qint64 writeData(const char *, qint64) override
    {
        return -1;
    }
};

} // namespace anonymous

namespace playable {

int Tone::frequency() const
{
    return frequency_;
}

void Tone::setFrequency(int frequency)
{
    if (frequency < 0) { // I guess 0 is the "null" value
        emit frequencyChanged();
        return;
    }

    if (frequency_ != frequency) {
        frequency_ = frequency;
        emit frequencyChanged();
    }
}

Tone::Tone(QObject *parent) : PlayableBase(parent)
{

}

namespace {

template <typename SampleType>
void writeSquareWaveData(float amplitude, SampleType minVal, SampleType maxVal,
                         size_t periodLength, size_t channelCount,
                         std::vector<char>& output)
{
    std::vector<SampleType> samples(periodLength * channelCount);
    const size_t sampleSizeInBytes = samples.size() * sizeof(SampleType);
    const size_t MB_16{1024 * 1024 * 16};
    if (sampleSizeInBytes > MB_16) {
        qWarning("Square wave samples length way larger than expected");
        return;
    }

    for (unsigned frame = 0; frame < periodLength; ++frame) {
        for (unsigned channel = 0; channel < channelCount; ++channel) {
            if (frame < periodLength / 2) {
                samples[frame * channelCount + channel] = minVal * amplitude;
            } else {
                samples[frame * channelCount + channel] = maxVal * amplitude;
            }
        }
    }

    output.resize(sampleSizeInBytes);
    memcpy(output.data(), samples.data(), output.size());
}

} // namespace anonymous

QIODevice *Tone::createPlayableDevice(const QAudioFormat &outputFormat)
{
    setError("");

    if (frequency_ <= 0) {
        setError("Invalid frequency");
        return nullptr;
    }

    if (!outputFormat.isValid()) {
        setError("Audio format is not valid");
        return nullptr;
    }
    Q_ASSERT(outputFormat.channelCount() >= 0);

    if (outputFormat.byteOrder() != QAudioFormat::LittleEndian) {
        setError("Only little endian is supported");
        return nullptr;
    }

    const float amplitude = 0.1f; // TODO Hard-coding sucks
    const int sampleRate = outputFormat.sampleRate(); // Samples per second
    const double periodInSamples = 1.0 / frequency_ * sampleRate;
    const unsigned periodInSamplesInteger = unsigned(periodInSamples);

    std::vector<char> sampleBytes;

    // These are the common ones, I think.
    if (outputFormat.sampleType() == QAudioFormat::Float) {
        if (outputFormat.sampleSize() == 32) {
            writeSquareWaveData<float>(amplitude, -1, 1, periodInSamplesInteger,
                                outputFormat.channelCount(), sampleBytes);
        } else {
            setError("Only 32 bits supported for float");
            return nullptr;
        }
    } else if (outputFormat.sampleType() == QAudioFormat::SignedInt) {
        if (outputFormat.sampleSize() == 16) {
            writeSquareWaveData<short>(amplitude, std::numeric_limits<short>::min(),
                                std::numeric_limits<short>::max(),
                                periodInSamplesInteger,
                                outputFormat.channelCount(), sampleBytes);
        } else if (outputFormat.sampleSize() == 32) {
            writeSquareWaveData<int>(amplitude, std::numeric_limits<int>::min(),
                              std::numeric_limits<int>::max(),
                              periodInSamplesInteger,
                              outputFormat.channelCount(), sampleBytes);
        } else {
            setError("Only 16 or 32 bits supported for signed int");
            return nullptr;
        }
    } else {
        setError("Only signed int and float are supported");
        return nullptr;
    }

    return new LoopedBytesIODevice(std::move(sampleBytes));
}

bool Tone::isFinite() const
{
    return false;
}

} // namespace playable

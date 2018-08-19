#include "samplemodifyingiodevice.h"

#include <cstring>
#include <QDebug>

qint64 SampleModifyingIODevice::readFromBufferAndIODevice(
    char* output, qint64 outputBytes, qint64 multiplesOf)
{
    Q_ASSERT(outputBytes % multiplesOf == 0);

    const qint64 bytesComingFromBuffer = qMin<qint64>(buffer_.size(), outputBytes);
    const qint64 bytesComingFromDevice = outputBytes - bytesComingFromBuffer;

    memcpy(output, buffer_.data(), bytesComingFromBuffer);
    buffer_.erase(buffer_.begin(), buffer_.begin() + bytesComingFromBuffer);

    const qint64 readLen =
        inferior_->read(output + bytesComingFromBuffer, bytesComingFromDevice);

    qint64 bytesWeHaveWritten;
    if (readLen >= 0) {
        bytesWeHaveWritten = bytesComingFromBuffer + readLen;
    } else {
        inferiorFlaggedError_ = true;
        bytesWeHaveWritten = bytesComingFromBuffer;
    }

    const qint64 bytesWeShouldHaveWritten = bytesWeHaveWritten / multiplesOf * multiplesOf;
    std::copy(output + bytesWeShouldHaveWritten, output + bytesWeHaveWritten,
              std::back_inserter(buffer_));
    return bytesWeShouldHaveWritten;
}

void SampleModifyingIODevice::read2FromBufferAndIODevice(
    char* output1, qint64 output1len, qint64* output1read,
    char* output2, qint64 output2len, qint64* output2read,
    qint64 multiplesOf)
{
    *output1read = readFromBufferAndIODevice(output1, output1len, multiplesOf);
    *output2read = 0;

    if (inferiorFlaggedError_ || *output1read != output1len) {
        // No-op: Can't fill output1, so don't try output2
    } else {
        *output2read = readFromBufferAndIODevice(output2, output2len, multiplesOf);
    }
}

qint64 SampleModifyingIODevice::readData(char *data, qint64 maxlen)
{
    if (!maxlen) {
        return 0;
    }
    if (inferiorFlaggedError_) {
        return -1;
    }

    const qint64 unitsRequested = maxlen / bytesPerUnit_;
    const qint64 unitsRequestedLen = unitsRequested * bytesPerUnit_;
    const qint64 partialLen = maxlen - unitsRequestedLen;

    std::vector<char> extraUnit;
    if (partialLen > 0) {
        extraUnit.resize(bytesPerUnit_);
    }

    qint64 directReadBytes = 0;
    qint64 extraUnitReadBytes = 0;

    read2FromBufferAndIODevice(
        data, unitsRequestedLen, &directReadBytes,
        extraUnit.data(), extraUnit.size(), &extraUnitReadBytes,
                bytesPerUnit_);

    qint64 retBytes = 0;

    {
        Q_ASSERT(directReadBytes % bytesPerUnit_ == 0);
        modifierFn_(data, directReadBytes / bytesPerUnit_, bytesPerUnit_);
        retBytes += directReadBytes;
    }

    if (extraUnitReadBytes) {
        Q_ASSERT(extraUnitReadBytes == bytesPerUnit_);
        modifierFn_(extraUnit.data(), 1, bytesPerUnit_);

        Q_ASSERT(buffer_.empty());

        std::copy_n(extraUnit.begin(), partialLen, data + unitsRequestedLen);
        std::copy(extraUnit.begin() + partialLen,
                  extraUnit.end(),
                  std::back_inserter(buffer_));

        retBytes += partialLen;
    }

    return retBytes;
}

qint64 SampleModifyingIODevice::writeData(const char *data, qint64 len)
{
    Q_ASSERT(false); // Not impl
    return -1;
}

bool SampleModifyingIODevice::open(QIODevice::OpenMode mode)
{
    if (inferior_->open(mode)) {
        // Base open never fails. Not worth supporting handling of base failure.
        QIODevice::open(mode);
        return true;
    } else {
        setErrorString(QString("Inferior device failed to open: %1")
                           .arg(inferior_->errorString()));
        return false;
    }
}

void SampleModifyingIODevice::close()
{
    if (inferior_->isOpen()) {
        inferior_->close();
    }
    QIODevice::close();
}

SampleModifyingIODevice::SampleModifyingIODevice(
    QIODevice* inferior, unsigned bytesPerUnit,
    SampleModifyingIODevice::ModifierFunction modifierFn)
    : inferior_(inferior), bytesPerUnit_(bytesPerUnit), modifierFn_(modifierFn)
{
    // buffer_.resize(bytesPerUnit_);
}

SampleModifyingIODevice::~SampleModifyingIODevice()
{
    delete inferior_;
}

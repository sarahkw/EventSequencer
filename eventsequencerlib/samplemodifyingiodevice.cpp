#include "samplemodifyingiodevice.h"

#include <cstring>
#include <QDebug>

namespace {
struct StartReadingFromBuffer {
    StartReadingFromBuffer(std::vector<char>& buffer, char* target, qint64 targetBytes)
    {
        const qint64 bytesComingFromBuffer = qMin<qint64>(buffer.size(), targetBytes);
        const qint64 bytesComingFromDevice = targetBytes - bytesComingFromBuffer;

        memcpy(target, buffer.data(), bytesComingFromBuffer);
        buffer.erase(buffer.begin(), buffer.begin() + bytesComingFromBuffer);

        this->bytesFromBuffer = bytesComingFromBuffer;
        this->bytesStillNeeded = bytesComingFromDevice;
        this->continueWritingAt = target + bytesComingFromBuffer;
    }
    qint64 bytesFromBuffer;
    qint64 bytesStillNeeded;
    char* continueWritingAt;
};
} // namespace anonymous

qint64 SampleModifyingIODevice::readFromBufferAndIODevice(
    char* output, qint64 outputBytes, qint64 multiplesOf)
{
    Q_ASSERT(outputBytes % multiplesOf == 0);

    qint64 bytesWeHaveWritten = inferior_->read(output, outputBytes);

    if (bytesWeHaveWritten >= 0) {
        const qint64 bytesWeShouldHaveWritten = bytesWeHaveWritten / multiplesOf * multiplesOf;

        auto from = std::reverse_iterator<char*>(output + bytesWeHaveWritten);
        auto to = std::reverse_iterator<char*>(output + bytesWeShouldHaveWritten);
        for (; from != to; ++from) {
            inferior_->ungetChar(*from);
        }

        return bytesWeShouldHaveWritten;
    } else {
        inferiorFlaggedError_ = true;
        return 0;
    }
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

    const StartReadingFromBuffer srfb(buffer_, data, maxlen);

    const qint64 unitsRequested = srfb.bytesStillNeeded / bytesPerUnit_;
    const qint64 unitsRequestedLen = unitsRequested * bytesPerUnit_;
    const qint64 partialLen = srfb.bytesStillNeeded - unitsRequestedLen;

    std::vector<char> extraUnit;
    if (partialLen > 0) {
        extraUnit.resize(bytesPerUnit_);
    }

    qint64 directReadBytes = 0;
    qint64 extraUnitReadBytes = 0;

    read2FromBufferAndIODevice(
        srfb.continueWritingAt, unitsRequestedLen, &directReadBytes,
        extraUnit.data(), extraUnit.size(), &extraUnitReadBytes,
                bytesPerUnit_);

    qint64 retBytes = srfb.bytesFromBuffer;

    {
        Q_ASSERT(directReadBytes % bytesPerUnit_ == 0);
        modifierFn_(data, directReadBytes / bytesPerUnit_, bytesPerUnit_);
        retBytes += directReadBytes;
    }

    if (extraUnitReadBytes) {
        Q_ASSERT(extraUnitReadBytes == bytesPerUnit_);
        modifierFn_(extraUnit.data(), 1, bytesPerUnit_);

        Q_ASSERT(buffer_.empty());

        std::copy_n(extraUnit.begin(), partialLen, srfb.continueWritingAt + unitsRequestedLen);
        std::copy(extraUnit.begin() + partialLen,
                  extraUnit.end(),
                  std::back_inserter(buffer_));

        retBytes += partialLen;
    }

    return retBytes;
}

qint64 SampleModifyingIODevice::writeData(const char *data, qint64 len)
{
    Q_ASSERT(len >= 0);
    if (incompleteWriteBuffer_.size() > 0) {
        qint64 writtenLen = inferior_->write(incompleteWriteBuffer_.data(),
                                             incompleteWriteBuffer_.size());
        const bool success = writtenLen == incompleteWriteBuffer_.size();
        qint64 bytesWritten = qMax<qint64>(writtenLen, 0);
        incompleteWriteBuffer_.erase(incompleteWriteBuffer_.begin(),
                                     incompleteWriteBuffer_.begin() + bytesWritten);
        if (writtenLen == -1) {
            return -1;
        } else if (!success) {
            return 0; // We wrote none of what was requested
        }
    }

    // We have to make a copy because we can't modify data directly.
    std::vector<char> modifyBuffer;
    modifyBuffer.reserve(buffer_.size() + len);
    std::copy(buffer_.begin(), buffer_.end(), std::back_inserter(modifyBuffer));
    buffer_.clear();
    std::copy_n(data, len, std::back_inserter(modifyBuffer));

    const qint64 fullUnits = modifyBuffer.size() / bytesPerUnit_;
    const qint64 fullUnitsBytes = fullUnits * bytesPerUnit_;

    char* writePtr = modifyBuffer.data();

    modifierFn_(writePtr, fullUnits, bytesPerUnit_);

    qint64 written = inferior_->write(writePtr, fullUnitsBytes);
    qint64 bytesWritten = qMax<qint64>(written, 0);

    std::copy(writePtr + bytesWritten, writePtr + fullUnitsBytes, std::back_inserter(incompleteWriteBuffer_));
    std::copy(writePtr + fullUnitsBytes, writePtr + modifyBuffer.size(), std::back_inserter(buffer_));

    // Unfortunately, we can't really handle errors here, since we have already
    // fed the data to modifierFn_. An error will be handled on next write, I guess.
    return len;
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
    // TODO If we're writing, flush the buffer.

    if (inferior_->isOpen()) {
        inferior_->close();
    }
    QIODevice::close();
}

bool SampleModifyingIODevice::flush()
{
    if ((openMode() & QIODevice::WriteOnly) == 0) {
        return true;
    }

    // TODO Implement!
    return false;
}

SampleModifyingIODevice::SampleModifyingIODevice(
    std::shared_ptr<QIODevice> inferior, unsigned bytesPerUnit,
    SampleModifyingIODevice::ModifierFunction modifierFn)
    : inferior_(inferior), bytesPerUnit_(bytesPerUnit), modifierFn_(modifierFn)
{
    // buffer_.resize(bytesPerUnit_);
}

SampleModifyingIODevice::~SampleModifyingIODevice()
{
}

#include "samplemodifyingiodevice.h"

#include <cstring>
#include <QDebug>

#ifdef COVERAGE
#define COVERAGE_COOKIE(id) qInfo(id)
#define COVERAGE_COOKIE_COND(cond, id) \
    do {                               \
        if (cond) {                    \
            qInfo(id);                 \
        }                              \
    } while (false)
#else
#define COVERAGE_COOKIE(id)
#define COVERAGE_COOKIE_COND(cond, id)
#endif

namespace {
struct StartReadingFromBuffer {
    StartReadingFromBuffer(std::vector<char>& buffer, char* target, qint64 targetBytes)
    {
        const qint64 bytesComingFromBuffer = qMin(qint64(buffer.size()), targetBytes);
        const qint64 bytesComingFromDevice = targetBytes - bytesComingFromBuffer;

        COVERAGE_COOKIE_COND((bytesComingFromBuffer > 0) && bytesComingFromDevice == 0, "COOKIE-34e91");
        COVERAGE_COOKIE_COND(bytesComingFromBuffer == 0 && (bytesComingFromDevice > 0), "COOKIE-34e92");
        COVERAGE_COOKIE_COND((bytesComingFromBuffer > 0) && (bytesComingFromDevice > 0), "COOKIE-34e93");
        // not-possible COVERAGE_COOKIE_COND(bytesComingFromBuffer == 0 && bytesComingFromDevice == 0, "COOKI-34e94");

        memcpy(target, buffer.data(), size_t(bytesComingFromBuffer));
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

    COVERAGE_COOKIE_COND(bytesWeHaveWritten == 0, "COOKIE-9067a");
    COVERAGE_COOKIE_COND(bytesWeHaveWritten == outputBytes, "COOKIE-9067b");
    COVERAGE_COOKIE_COND(bytesWeHaveWritten > 0 && bytesWeHaveWritten < outputBytes, "COOKIE-9067c");
    COVERAGE_COOKIE_COND(bytesWeHaveWritten == -1, "COOKIE-9067d");

    if (bytesWeHaveWritten >= 0) {
        COVERAGE_COOKIE("COOKIE-1cbf0");
        
        const qint64 bytesWeShouldHaveWritten = bytesWeHaveWritten / multiplesOf * multiplesOf;

        auto from = std::reverse_iterator<char*>(output + bytesWeHaveWritten);
        auto to = std::reverse_iterator<char*>(output + bytesWeShouldHaveWritten);
        for (; from != to; ++from) {
            COVERAGE_COOKIE("COOKIE-3c505");
            inferior_->ungetChar(*from);
        }

        return bytesWeShouldHaveWritten;
    } else {
        COVERAGE_COOKIE("COOKIE-34553");
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
        COVERAGE_COOKIE_COND(inferiorFlaggedError_, "COOKIE-242f5");
        COVERAGE_COOKIE_COND(!inferiorFlaggedError_ && *output1read != output1len, "COOKIE-19c0f");
        COVERAGE_COOKIE_COND(!inferiorFlaggedError_ && *output1read < output1len, "COOKIE-19c10");
        // No-op: Can't fill output1, so don't try output2
    } else {
        COVERAGE_COOKIE("COOKIE-4f22b");
        *output2read = readFromBufferAndIODevice(output2, output2len, multiplesOf);
        COVERAGE_COOKIE_COND(inferiorFlaggedError_, "COOKIE-16c18");
        COVERAGE_COOKIE_COND(!inferiorFlaggedError_ && *output2read != output2len, "COOKIE-16c19");
        COVERAGE_COOKIE_COND(!inferiorFlaggedError_ && *output2read < output2len, "COOKIE-16c1a");
    }
}

qint64 SampleModifyingIODevice::readData(char *data, qint64 maxlen)
{
    if (inferiorFlaggedError_) {
        COVERAGE_COOKIE("COOKIE-3007a");
        return -1;
    }
    if (!maxlen) {
        return 0;
    }

    const StartReadingFromBuffer srfb(buffer_, data, maxlen);

    const qint64 unitsRequested = srfb.bytesStillNeeded / bytesPerUnit_;
    const qint64 unitsRequestedLen = unitsRequested * bytesPerUnit_;
    const qint64 partialLen = srfb.bytesStillNeeded - unitsRequestedLen;

    std::vector<char> extraUnit;
    if (partialLen > 0) {
        COVERAGE_COOKIE("COOKIE-aef32");
        extraUnit.resize(bytesPerUnit_);
    }

    qint64 directReadBytes = 0;
    qint64 extraUnitReadBytes = 0;

    read2FromBufferAndIODevice(
        srfb.continueWritingAt, unitsRequestedLen, &directReadBytes,
        extraUnit.data(), qint64(extraUnit.size()), &extraUnitReadBytes,
                bytesPerUnit_);

    qint64 retBytes = srfb.bytesFromBuffer;

    {
        COVERAGE_COOKIE_COND(directReadBytes == 0, "COOKIE-17eb0");
        COVERAGE_COOKIE_COND(directReadBytes > 0, "COOKIE-17eb1");
        Q_ASSERT(directReadBytes % bytesPerUnit_ == 0);
        modifierFn_(data, unsigned(directReadBytes / bytesPerUnit_), bytesPerUnit_);
        retBytes += directReadBytes;
    }

    if (extraUnitReadBytes) {
        COVERAGE_COOKIE("COOKIE-c2391");
        Q_ASSERT(extraUnitReadBytes == bytesPerUnit_);
        modifierFn_(extraUnit.data(), 1, bytesPerUnit_);

        Q_ASSERT(buffer_.empty());
        Q_ASSERT(partialLen > 0);

        std::copy_n(extraUnit.begin(), partialLen, srfb.continueWritingAt + unitsRequestedLen);
        std::copy(extraUnit.begin() + partialLen,
                  extraUnit.end(),
                  std::back_inserter(buffer_));

        retBytes += partialLen;
    }

    if (retBytes == 0 && inferiorFlaggedError_) {
        COVERAGE_COOKIE("COOKIE-1dc69");
        return -1;
    }
    COVERAGE_COOKIE_COND(inferiorFlaggedError_ && retBytes > 0, "COOKIE-b241c");

    return retBytes;
}

qint64 SampleModifyingIODevice::writeData(const char *data, qint64 len)
{
    Q_ASSERT(len >= 0);
    if (incompleteWriteBuffer_.size() > 0) {
        COVERAGE_COOKIE("COOKIE-20cc3");
        qint64 writtenLen = inferior_->write(incompleteWriteBuffer_.data(),
                                             qint64(incompleteWriteBuffer_.size()));
        const bool success = writtenLen == qint64(incompleteWriteBuffer_.size());
        COVERAGE_COOKIE_COND(writtenLen > 0 && writtenLen < incompleteWriteBuffer_.size(), "COOKIE-26b5a");
        qint64 bytesWritten = qMax<qint64>(writtenLen, 0);
        incompleteWriteBuffer_.erase(incompleteWriteBuffer_.begin(),
                                     incompleteWriteBuffer_.begin() + bytesWritten);
        COVERAGE_COOKIE_COND(bytesWritten > 0, "COOKIE-394e1");
        COVERAGE_COOKIE_COND(bytesWritten == 0, "COOKIE-394e2");
        if (writtenLen == -1) {
            COVERAGE_COOKIE("COOKIE-1a358");
            return -1;
        } else if (!success) {
            COVERAGE_COOKIE("COOKIE-9fe37");
            return 0; // We wrote none of what was requested
        }
    }

    COVERAGE_COOKIE_COND(buffer_.size() > 0, "COOKIE-2b91d");
    COVERAGE_COOKIE_COND(buffer_.size() == 0, "COOKIE-2b91e");

    // We have to make a copy because we can't modify data directly.
    std::vector<char> modifyBuffer;
    modifyBuffer.reserve(buffer_.size() + size_t(len));
    std::copy(buffer_.begin(), buffer_.end(), std::back_inserter(modifyBuffer));
    buffer_.clear();
    std::copy_n(data, len, std::back_inserter(modifyBuffer));

    const qint64 fullUnits = qint64(modifyBuffer.size() / bytesPerUnit_);
    const qint64 fullUnitsBytes = fullUnits * bytesPerUnit_;

    char* writePtr = modifyBuffer.data();

    modifierFn_(writePtr, unsigned(fullUnits), bytesPerUnit_);

    qint64 written = inferior_->write(writePtr, fullUnitsBytes);
    qint64 bytesWritten = qMax<qint64>(written, 0);

    COVERAGE_COOKIE_COND(written > 0 && written < fullUnitsBytes, "COOKIE-273d8");
    COVERAGE_COOKIE_COND(written == -1, "COOKIE-1e042");

    COVERAGE_COOKIE_COND(fullUnitsBytes > bytesWritten, "COOKIE-38ca0");
    //not-possible COVERAGE_COOKIE_COND(fullUnitsBytes < bytesWritten, "COOKI-38ca1");
    COVERAGE_COOKIE_COND(fullUnitsBytes == bytesWritten, "COOKIE-38ca2");

    //not-possible COVERAGE_COOKIE_COND(fullUnitsBytes > modifyBuffer.size(), "COOKI-34ef5");
    COVERAGE_COOKIE_COND(fullUnitsBytes < modifyBuffer.size(), "COOKIE-34ef6");
    COVERAGE_COOKIE_COND(fullUnitsBytes == modifyBuffer.size(), "COOKIE-34ef7");

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
    if (!flush()) {
        qWarning("SampleModifyingIODevice flush on close failed");
    }

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

    if (!incompleteWriteBuffer_.empty()) {
        auto result = inferior_->write(incompleteWriteBuffer_.data(),
                                       qint64(incompleteWriteBuffer_.size()));
        const bool allWritten = result == qint64(incompleteWriteBuffer_.size());
        if (result > 0) {
            incompleteWriteBuffer_.erase(incompleteWriteBuffer_.begin(),
                                         incompleteWriteBuffer_.begin() + result);
        }

        COVERAGE_COOKIE_COND(result > 0, "COOKIE-3c3d1");
        COVERAGE_COOKIE_COND(result < 0, "COOKIE-3c3d2");
        COVERAGE_COOKIE_COND(result == 0, "COOKIE-3c3d3");

        COVERAGE_COOKIE_COND(allWritten, "COOKIE-2181d");
        COVERAGE_COOKIE_COND(!allWritten, "COOKIE-2181e");
        COVERAGE_COOKIE_COND(result > 0 && !allWritten, "COOKIE-1562a");

        return allWritten;
    } else {
        return true;
    }
}

size_t SampleModifyingIODevice::incompleteByteCount() const
{
    return buffer_.size();
}

SampleModifyingIODevice::SampleModifyingIODevice(
    std::shared_ptr<QIODevice> inferior, unsigned bytesPerUnit,
    SampleModifyingIODevice::ModifierFunction modifierFn)
    : inferior_(inferior), bytesPerUnit_(bytesPerUnit), modifierFn_(modifierFn)
{
}

SampleModifyingIODevice::~SampleModifyingIODevice()
{
}

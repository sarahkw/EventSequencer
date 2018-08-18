#include "samplemodifyingiodevice.h"

#include <cstring>
#include <QDebug>

qint64 SampleModifyingIODevice::readFromBufferAndIODevice(char* output, qint64 maxlen)
{
    const qint64 bytesComingFromBuffer = qMin<qint64>(buffer_.size(), maxlen);
    const qint64 bytesComingFromDevice = maxlen - bytesComingFromBuffer;

    memcpy(output, buffer_.data(), bytesComingFromBuffer);
    buffer_.erase(buffer_.begin(), buffer_.begin() + bytesComingFromBuffer);

    const qint64 readLen =
        inferior_->read(output + bytesComingFromBuffer, bytesComingFromDevice);

    if (readLen >= 0) {
        return bytesComingFromBuffer + readLen;
    } else {
        inferiorFlaggedError_ = true;
        return bytesComingFromBuffer;
    }
}

void SampleModifyingIODevice::read2FromBufferAndIODevice(
    char* output1, qint64 output1len, qint64* output1read,
    char* output2, qint64 output2len, qint64* output2read)
{
    // TODO if we know we're reading again, provide a hint to keep
    //      extra data in the buffer to avoid another read()
    
    *output1read = readFromBufferAndIODevice(output1, output1len);
    *output2read = 0;

    if (inferiorFlaggedError_ || *output1read != output1len) {
        // No-op: Can't fill output1, so don't try output2
    } else {
        if (output2 != nullptr) {
            *output2read = readFromBufferAndIODevice(output2, output2len);
        }
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

    const qint64 fullUnitsRequested = maxlen / bytesPerUnit_;
    const qint64 fullUnitsLen = fullUnitsRequested * bytesPerUnit_;
    const bool extraUnitWanted = fullUnitsLen < maxlen;

    std::vector<char> extraUnit;
    if (extraUnitWanted) {
        extraUnit.resize(bytesPerUnit_);
    }

    qint64 fullUnitRead = 0;
    qint64 extraUnitRead = 0;

    read2FromBufferAndIODevice(
        data, fullUnitsLen, &fullUnitRead,
        extraUnit.data(), extraUnit.size(), &extraUnitRead);

    if (fullUnitsLen == fullUnitRead) {
        if (extraUnitWanted) {
            if (extraUnit.size() == extraUnitRead) {
                modifierFn_(data, fullUnitsRequested, bytesPerUnit_,
                            extraUnit.data());

                qint64 bytesInside = maxlen - fullUnitsLen;
                Q_ASSERT(bytesInside > 0);
                Q_ASSERT(bytesInside < bytesPerUnit_);

                std::copy_n(extraUnit.begin(), bytesInside, data + fullUnitsLen);
                std::copy(extraUnit.begin() + bytesInside,
                          extraUnit.end(),
                          std::back_inserter(buffer_));
                return maxlen;
            } else {
                // EOF while reading extra unit
                modifierFn_(data, fullUnitsRequested, bytesPerUnit_, nullptr);
                std::copy_n(extraUnit.begin(), extraUnitRead, std::back_inserter(buffer_));
                Q_ASSERT(fullUnitsLen == maxlen);
                return fullUnitsLen;
            }
        } else {
            modifierFn_(data, fullUnitsRequested, bytesPerUnit_, nullptr);
            return fullUnitsLen;
        }
    } else {
        const qint64 aaa = fullUnitRead / bytesPerUnit_;
        const qint64 bbb = aaa * bytesPerUnit_;
        const qint64 ccc = fullUnitRead - bbb;
        
        modifierFn_(data, aaa, bytesPerUnit_, nullptr);
        std::copy(data + bbb, data + ccc, std::back_inserter(buffer_));
        return bbb;
    }
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

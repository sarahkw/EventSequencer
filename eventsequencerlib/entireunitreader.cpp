#include "entireunitreader.h"

#include <type_traits>

size_t EntireUnitReader::readBytes(const QtReadFn &readFn,
                                   char* data, size_t datalen, size_t unitSize,
                                   EntireUnitReader::Error* error)
{
    size_t totalBytesRead = 0;
    std::underlying_type_t<Error> myError{};

    while (datalen > 0) {
        const auto bytesRead = readFn(data, qint64(datalen));
        if (bytesRead <= 0) {
            if (bytesRead == -1) {
                myError |= Error::NegativeOneError;
            }
            break;
        } else {
            Q_ASSERT(bytesRead > 0);
            const size_t bytesReadSize = size_t(bytesRead);
            totalBytesRead += bytesReadSize;
            data += bytesReadSize;
            Q_ASSERT(datalen >= bytesReadSize);
            datalen -= bytesReadSize;
        }
    }

    const auto fullUnitsRead = totalBytesRead / unitSize;
    const auto leftOverBytes = totalBytesRead % unitSize;
    if (leftOverBytes > 0) {
        myError |= Error::DiscardedDataDueToEofError;
    }

    if (error != nullptr) {
        *error = static_cast<Error>(myError);
    }

    return fullUnitsRead * unitSize;
}

#ifndef ENTIREUNITREADER_H
#define ENTIREUNITREADER_H

#include <QtGlobal>

#include <functional>

class EntireUnitReader
{
    EntireUnitReader() = delete;
public:

    // Qt style read fn. EOF condition will be if it returns 0, error condition
    // will be if it returns -1.
    using QtReadFn = std::function<qint64 (char*, qint64)>;

    enum Error {
        NoError = 0,
        DiscardedDataDueToEofError = 1,
        NegativeOneError = 2
    };

    // EOF condition: return value < datalen
    static size_t readBytes(const QtReadFn& readFn, char* data, size_t datalen,
                            size_t unitSize, Error* error = nullptr);

    // EOF condition: return value < datalen
    template <typename T>
    static size_t readUnitGroups(const QtReadFn& readFn, T* data,
                                 size_t datalen, size_t unitGroupCount,
                                 size_t unitGroupSize = 1,
                                 Error* error = nullptr)
    {
        Q_ASSERT(unitGroupCount * unitGroupSize <= datalen);
        return readBytes(readFn, reinterpret_cast<char*>(data),
                         sizeof(T) * unitGroupCount * unitGroupSize,
                         sizeof(T) * unitGroupSize, error) /
               (sizeof(T) * unitGroupSize);
    }
};

#endif // ENTIREUNITREADER_H

#ifndef SAMPLEMODIFYINGIODEVICE_H
#define SAMPLEMODIFYINGIODEVICE_H

#include <QIODevice>
#include <functional>
#include <memory>

class SampleModifyingIODevice : public QIODevice
{
    // Is shared_ptr because if we're a writer, we shouldn't need to take
    // ownership.
    std::shared_ptr<QIODevice> inferior_;
    unsigned bytesPerUnit_;

    std::vector<char> buffer_;
    bool inferiorFlaggedError_ = false;

    std::vector<char> incompleteWriteBuffer_;

    qint64 readFromBufferAndIODevice(
        char* output, qint64 outputBytes, qint64 multiplesOf);
    void read2FromBufferAndIODevice(
        char* output1, qint64 output1len, qint64* output1read,
        char* output2, qint64 output2len, qint64* output2read,
        qint64 multiplesOf);

    Q_DISABLE_COPY(SampleModifyingIODevice)

public:
    using ModifierFunction = std::function<void(char* data, unsigned dataUnits,
                                                unsigned bytesPerUnit)>;

private:
    ModifierFunction modifierFn_;

protected:

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

public:

    bool open(OpenMode mode) override;
    void close() override;
    bool flush();
    size_t incompleteByteCount() const;

    SampleModifyingIODevice(std::shared_ptr<QIODevice> inferior,
                            unsigned bytesPerUnit, ModifierFunction modifierFn);
};

#endif // SAMPLEMODIFYINGIODEVICE_H

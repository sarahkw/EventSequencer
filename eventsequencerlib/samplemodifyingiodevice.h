#ifndef SAMPLEMODIFYINGIODEVICE_H
#define SAMPLEMODIFYINGIODEVICE_H

#include <QIODevice>
#include <functional>

class SampleModifyingIODevice : public QIODevice
{
    QIODevice* inferior_;
    unsigned bytesPerUnit_;
    std::vector<char> incompleteBuffer_;

    Q_DISABLE_COPY(SampleModifyingIODevice)

public:
    // Returns number of units consumed.
    using ModifierFunction = std::function<unsigned(char* data, unsigned dataUnits, unsigned bytesPerUnit)>;
private:
    ModifierFunction modifierFn_;

protected:

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    bool open(OpenMode mode) override;
    void close() override;

public:
    // Takes ownership of inferior.
    SampleModifyingIODevice(QIODevice* inferior, unsigned bytesPerUnit, ModifierFunction modifierFn);
    ~SampleModifyingIODevice() override;
};

#endif // SAMPLEMODIFYINGIODEVICE_H

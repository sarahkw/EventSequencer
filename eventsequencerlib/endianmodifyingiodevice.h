#ifndef ENDIANMODIFYINGIODEVICE_H
#define ENDIANMODIFYINGIODEVICE_H

#include <samplemodifyingiodevice.h>

class EndianModifyingIODevice : public SampleModifyingIODevice
{
public:

    enum Endian {
        Big,
        Little
    };

    EndianModifyingIODevice(std::shared_ptr<QIODevice> inferior,
                            unsigned bytesPerUnit,
                            Endian source,
                            Endian destination);
};

#endif // ENDIANMODIFYINGIODEVICE_H

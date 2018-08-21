#include "endianmodifyingiodevice.h"

namespace {
void reverse(char* data, unsigned dataUnits, unsigned bytesPerUnit)
{
    for (unsigned i = 0; i < dataUnits; ++i) {
        std::reverse(data + i * bytesPerUnit, data + (i + 1) * bytesPerUnit);
    }
}

void noop(char*, unsigned, unsigned) {
}

SampleModifyingIODevice::ModifierFunction whichFunction(
    EndianModifyingIODevice::Endian source,
    EndianModifyingIODevice::Endian destination)
{
    if (source != destination) {
        return reverse;
    } else {
        return noop;
    }
}
}

EndianModifyingIODevice::EndianModifyingIODevice(
    std::shared_ptr<QIODevice> inferior, unsigned bytesPerUnit,
    EndianModifyingIODevice::Endian source,
    EndianModifyingIODevice::Endian destination)
    : SampleModifyingIODevice(inferior, bytesPerUnit,
                              whichFunction(source, destination))
{
}

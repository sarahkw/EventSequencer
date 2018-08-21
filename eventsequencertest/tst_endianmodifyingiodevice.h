#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <endianmodifyingiodevice.h>

#include <QtEndian>
#include <QBuffer>

#include <memory>

TEST(EndianModifyingIODevice, Integers_Swap)
{
    static_assert(Q_LITTLE_ENDIAN, "Test only supported on LE machine");

    std::vector<unsigned> samplesLE{
        0x12345678, 0xAABBCCDD
    };

    std::vector<unsigned> samplesBE = samplesLE;
    for (auto& s : samplesBE) {
        s = qToBigEndian(s);
    }

    QByteArray bytesLE(reinterpret_cast<const char*>(samplesLE.data()),
                       int(samplesLE.size() * sizeof(unsigned)));

    QByteArray bytesBE(reinterpret_cast<const char*>(samplesBE.data()),
                       int(samplesBE.size() * sizeof(unsigned)));

    auto inferior = std::make_shared<QBuffer>(&bytesLE);

    EndianModifyingIODevice emiod(inferior, sizeof(unsigned),
                                  EndianModifyingIODevice::Little,
                                  EndianModifyingIODevice::Big);

    ASSERT_TRUE(emiod.open(QIODevice::ReadOnly));

    EXPECT_EQ(bytesBE, emiod.readAll());
}

TEST(EndianModifyingIODevice, Integers_Same)
{
    std::vector<unsigned> samplesLE{
        0x12345678, 0xAABBCCDD
    };

    QByteArray bytesLE(reinterpret_cast<const char*>(samplesLE.data()),
                       int(samplesLE.size() * sizeof(unsigned)));

    auto inferior = std::make_shared<QBuffer>(&bytesLE);

    EndianModifyingIODevice emiod(inferior, sizeof(unsigned),
                                  EndianModifyingIODevice::Little,
                                  EndianModifyingIODevice::Little);

    ASSERT_TRUE(emiod.open(QIODevice::ReadOnly));

    EXPECT_EQ(bytesLE, emiod.readAll());
}

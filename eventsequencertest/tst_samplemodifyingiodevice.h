#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <samplemodifyingiodevice.h>

#include <QBuffer>

inline void PrintTo(const QString& s, ::std::ostream *os)
{
    *os << s.toStdString();
}

TEST(SampleModifyingIODevice, BasicRead)
{
    QByteArray input = QString("FEEDME").toUtf8();

    const auto modifyFn = [](char* data, unsigned dataUnits,
                             unsigned bytesPerUnit) {

        for (unsigned i = 0; i < dataUnits; ++i) {
            std::reverse(data + i * bytesPerUnit,
                         data + (i + 1) * bytesPerUnit);
        }
    };

    SampleModifyingIODevice smiod(new QBuffer(&input), 2, modifyFn);
    ASSERT_TRUE(smiod.open(QIODevice::ReadOnly));
    QString str(smiod.readAll());
    EXPECT_EQ(str, "EFDEEM");
    smiod.close();
}

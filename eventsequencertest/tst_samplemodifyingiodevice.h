#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <samplemodifyingiodevice.h>

#include <QBuffer>

inline void PrintTo(const QString& s, ::std::ostream *os)
{
    *os << s.toStdString();
}


const auto reverseFn = [](char* data, unsigned dataUnits,
                          unsigned bytesPerUnit) {
    for (unsigned i = 0; i < dataUnits; ++i) {
        std::reverse(data + i * bytesPerUnit, data + (i + 1) * bytesPerUnit);
    }
};

TEST(SampleModifyingIODevice, BasicRead)
{
    QByteArray input = QString("FEEDME").toUtf8();

    SampleModifyingIODevice smiod(std::make_shared<QBuffer>(&input), 2, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::ReadOnly));
    QString str(smiod.readAll());
    EXPECT_EQ(str, "EFDEEM");
    smiod.close();
}

TEST(SampleModifyingIODevice, Write_2_Basic)
{
    QByteArray input = QString("AUTUMN").toUtf8();

    auto output = std::make_shared<QBuffer>();
    ASSERT_TRUE(output->open(QIODevice::WriteOnly));

    SampleModifyingIODevice smiod(output, 3, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::WriteOnly));
    EXPECT_THAT(smiod.write(input), input.size());

    QString str(output->data());
    EXPECT_EQ(str, "TUANMU");
}

TEST(SampleModifyingIODevice, Write_2_BiggerFirst)
{
    QString input1("AUTU"), input2("MN");

    auto output = std::make_shared<QBuffer>();
    ASSERT_TRUE(output->open(QIODevice::WriteOnly));

    SampleModifyingIODevice smiod(output, 3, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::WriteOnly));

    EXPECT_THAT(smiod.write(input1.toUtf8()), input1.size());
    EXPECT_EQ(QString(output->data()), "TUA");
    EXPECT_THAT(smiod.write(input2.toUtf8()), input2.size());
    EXPECT_EQ(QString(output->data()), "TUANMU");
}

TEST(SampleModifyingIODevice, Write_2_SmallerFirst)
{
    QString input1("AU"), input2("TUMN");

    auto output = std::make_shared<QBuffer>();
    ASSERT_TRUE(output->open(QIODevice::WriteOnly));

    SampleModifyingIODevice smiod(output, 3, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::WriteOnly));

    EXPECT_THAT(smiod.write(input1.toUtf8()), input1.size());
    EXPECT_EQ(QString(output->data()), "");
    EXPECT_THAT(smiod.write(input2.toUtf8()), input2.size());
    EXPECT_EQ(QString(output->data()), "TUANMU");
}

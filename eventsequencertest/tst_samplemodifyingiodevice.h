#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <samplemodifyingiodevice.h>

#include <QBuffer>

inline void PrintTo(const QString& s, ::std::ostream *os)
{
    *os << s.toStdString();
}

struct MockIODevice : public QIODevice {
    MOCK_METHOD2(readData  ,qint64(char *data, qint64 maxlen));
    MOCK_METHOD2(writeData ,qint64(const char *data, qint64 len));
};

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

TEST(SampleModifyingIODevice, Write_Mocked_Fail_0)
{
    using testing::Return;
    using testing::StartsWith;

    auto output = std::make_shared<MockIODevice>();
    ASSERT_TRUE(output->open(QIODevice::WriteOnly));

    SampleModifyingIODevice smiod(output, 3, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::WriteOnly));

    {
        EXPECT_CALL(*output, writeData(StartsWith("LEH"), 3)).WillOnce(Return(0));
        QString chunk("HELLO");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), chunk.size());
    }

    {
        EXPECT_CALL(*output, writeData(StartsWith("LEH"), 3)).WillOnce(Return(3));
        EXPECT_CALL(*output, writeData(StartsWith("!OL"), 3)).WillOnce(Return(3));
        QString chunk("!");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), chunk.size());
    }
}

TEST(SampleModifyingIODevice, Write_Mocked_Failures_1)
{
    using testing::Return;
    using testing::StartsWith;

    auto output = std::make_shared<MockIODevice>();
    ASSERT_TRUE(output->open(QIODevice::WriteOnly));

    SampleModifyingIODevice smiod(output, 3, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::WriteOnly));

    {
        EXPECT_CALL(*output, writeData(StartsWith("LEH"), 3)).WillOnce(Return(-1));
        QString chunk("HELLO");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), chunk.size());
    }

    {
        EXPECT_CALL(*output, writeData(StartsWith("LEH"), 3)).WillOnce(Return(-1));
        QString chunk("!");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), -1); // Cannot flush old buffer, don't take more
    }

    {
        EXPECT_CALL(*output, writeData(StartsWith("LEH"), 3)).WillOnce(Return(0));
        QString chunk("!");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), 0); // Don't give -1 unless inferior gives -1
    }

    {
        EXPECT_CALL(*output, writeData(StartsWith("LEH"), 3)).WillOnce(Return(1));
        QString chunk("!");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), 0); // Only able to write 1 from old buffer
    }

    {
        EXPECT_CALL(*output, writeData(StartsWith("EH"), 2)).WillOnce(Return(2));
        EXPECT_CALL(*output, writeData(StartsWith("!OL"), 3)).WillOnce(Return(2));
        QString chunk("!");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), 1);
    }

    {
        EXPECT_CALL(*output, writeData(StartsWith("L"), 1)).WillOnce(Return(0));
        EXPECT_FALSE(smiod.flush());
    }

    {
        EXPECT_CALL(*output, writeData(StartsWith("L"), 1)).WillOnce(Return(1));
        EXPECT_TRUE(smiod.flush());
    }

    {
        EXPECT_TRUE(smiod.flush());
    }
}

TEST(SampleModifyingIODevice, Write_Flush)
{
    using testing::Return;
    using testing::StartsWith;

    auto output = std::make_shared<MockIODevice>();
    ASSERT_TRUE(output->open(QIODevice::WriteOnly));

    SampleModifyingIODevice smiod(output, 3, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::WriteOnly));

    {
        EXPECT_CALL(*output, writeData(StartsWith("LEH!OL"), 6)).WillOnce(Return(-1));
        QString chunk("HELLO!!");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), chunk.size());
    }

    {
        EXPECT_CALL(*output, writeData(StartsWith("LEH!OL"), 6)).WillOnce(Return(-1));
        EXPECT_FALSE(smiod.flush());
    }

    {
        EXPECT_CALL(*output, writeData(StartsWith("LEH!OL"), 6)).WillOnce(Return(0));
        EXPECT_FALSE(smiod.flush());
    }

    {
        EXPECT_CALL(*output, writeData(StartsWith("LEH!OL"), 6)).WillOnce(Return(1));
        EXPECT_FALSE(smiod.flush());
    }

    {
        EXPECT_CALL(*output, writeData(StartsWith("EH!OL"), 5)).WillOnce(Return(5));
        EXPECT_TRUE(smiod.flush());
    }

    EXPECT_EQ(smiod.incompleteByteCount(), 1);

    {
        EXPECT_CALL(*output, writeData(StartsWith("21!"), 3)).WillOnce(Return(3));
        QString chunk("12");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), chunk.size());
    }

    EXPECT_EQ(smiod.incompleteByteCount(), 0);
}

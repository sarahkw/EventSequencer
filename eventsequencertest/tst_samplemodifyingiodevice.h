#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <samplemodifyingiodevice.h>

#include <printto_qstring.h>

#include <QBuffer>

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

MATCHER_P(MemEq, s, "") {
    return memcmp(s, arg, strlen(s)) == 0;
}

TEST(SampleModifyingIODevice, Write_Mocked_Fail_0)
{
    using testing::Return;

    auto output = std::make_shared<MockIODevice>();
    ASSERT_TRUE(output->open(QIODevice::WriteOnly));

    SampleModifyingIODevice smiod(output, 3, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::WriteOnly));

    {
        EXPECT_CALL(*output, writeData(MemEq("LEH"), 3)).WillOnce(Return(0));
        QString chunk("HELLO");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), chunk.size());
    }

    {
        EXPECT_CALL(*output, writeData(MemEq("LEH"), 3)).WillOnce(Return(3));
        EXPECT_CALL(*output, writeData(MemEq("!OL"), 3)).WillOnce(Return(3));
        QString chunk("!");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), chunk.size());
    }
}

TEST(SampleModifyingIODevice, Write_Mocked_Failures_1)
{
    using testing::Return;

    auto output = std::make_shared<MockIODevice>();
    ASSERT_TRUE(output->open(QIODevice::WriteOnly));

    SampleModifyingIODevice smiod(output, 3, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::WriteOnly));

    {
        EXPECT_CALL(*output, writeData(MemEq("LEH"), 3)).WillOnce(Return(-1));
        QString chunk("HELLO");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), chunk.size());
    }

    {
        EXPECT_CALL(*output, writeData(MemEq("LEH"), 3)).WillOnce(Return(-1));
        QString chunk("!");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), -1); // Cannot flush old buffer, don't take more
    }

    {
        EXPECT_CALL(*output, writeData(MemEq("LEH"), 3)).WillOnce(Return(0));
        QString chunk("!");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), 0); // Don't give -1 unless inferior gives -1
    }

    {
        EXPECT_CALL(*output, writeData(MemEq("LEH"), 3)).WillOnce(Return(1));
        QString chunk("!");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), 0); // Only able to write 1 from old buffer
    }

    {
        EXPECT_CALL(*output, writeData(MemEq("EH"), 2)).WillOnce(Return(2));
        EXPECT_CALL(*output, writeData(MemEq("!OL"), 3)).WillOnce(Return(2));
        QString chunk("!");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), 1);
    }

    {
        EXPECT_CALL(*output, writeData(MemEq("L"), 1)).WillOnce(Return(0));
        EXPECT_FALSE(smiod.flush());
    }

    {
        EXPECT_CALL(*output, writeData(MemEq("L"), 1)).WillOnce(Return(1));
        EXPECT_TRUE(smiod.flush());
    }

    {
        EXPECT_TRUE(smiod.flush());
    }
}

TEST(SampleModifyingIODevice, Write_Flush)
{
    using testing::Return;

    auto output = std::make_shared<MockIODevice>();
    ASSERT_TRUE(output->open(QIODevice::WriteOnly));

    SampleModifyingIODevice smiod(output, 3, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::WriteOnly));

    {
        EXPECT_CALL(*output, writeData(MemEq("LEH!OL"), 6)).WillOnce(Return(-1));
        QString chunk("HELLO!!");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), chunk.size());
    }

    {
        EXPECT_CALL(*output, writeData(MemEq("LEH!OL"), 6)).WillOnce(Return(-1));
        EXPECT_FALSE(smiod.flush());
    }

    {
        EXPECT_CALL(*output, writeData(MemEq("LEH!OL"), 6)).WillOnce(Return(0));
        EXPECT_FALSE(smiod.flush());
    }

    {
        EXPECT_CALL(*output, writeData(MemEq("LEH!OL"), 6)).WillOnce(Return(1));
        EXPECT_FALSE(smiod.flush());
    }

    {
        EXPECT_CALL(*output, writeData(MemEq("EH!OL"), 5)).WillOnce(Return(5));
        EXPECT_TRUE(smiod.flush());
    }

    EXPECT_EQ(smiod.incompleteByteCount(), 1);

    {
        EXPECT_CALL(*output, writeData(MemEq("21!"), 3)).WillOnce(Return(3));
        QString chunk("12");
        EXPECT_THAT(smiod.write(chunk.toUtf8()), chunk.size());
    }

    EXPECT_EQ(smiod.incompleteByteCount(), 0);
}

class TestRead : public QBuffer {
    QByteArray myData_;

    bool error_ = false;
    bool errorAfterLimitReached_ = false;
    qint64 currentBytesToRead_ = 9999999; // Don't do this in production ;)
public:
    TestRead(QString s) : QBuffer(&myData_)
    {
        myData_ = s.toUtf8();
    }

    void setLimit(qint64 limit) { currentBytesToRead_ = limit; }
    void resetLimit() { currentBytesToRead_ = 9999999; }
    void setError() { error_ = true; }
    void unsetError() { error_ = false; }
    void setErrorAfterLimitReached() { errorAfterLimitReached_ = true; }

    qint64 readData(char *data, qint64 maxlen) override
    {
        if (error_) return -1;
        if (currentBytesToRead_ == 0 && errorAfterLimitReached_) return -1;

        auto readlimit = qMin(maxlen, currentBytesToRead_);
        auto actualread = QBuffer::readData(data, readlimit);
        if (actualread > 0) {
            currentBytesToRead_ -= actualread;
        }
        return actualread;
    }
    qint64 writeData(const char *data, qint64 len) override { return -1; }
};

TEST(SampleModifyingIODevice, Read_Mocked_Basic)
{
    using testing::Invoke;
    using testing::Return;
    using testing::_;

    auto inferior = std::make_shared<TestRead>("HELLO!");
    ASSERT_TRUE(inferior->open(QIODevice::ReadOnly));

    SampleModifyingIODevice smiod(inferior, 3, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::ReadOnly));

    inferior->setLimit(4);
    EXPECT_EQ(smiod.readAll(), QString("LEH"));
    inferior->resetLimit();
    EXPECT_EQ(smiod.readAll(), QString("!OL"));
}

TEST(SampleModifyingIODevice, Read_Mocked_ExtraUnit)
{
    using testing::Invoke;
    using testing::Return;
    using testing::_;

    auto inferior = std::make_shared<TestRead>("HELLO!");
    ASSERT_TRUE(inferior->open(QIODevice::ReadOnly));

    SampleModifyingIODevice smiod(inferior, 3, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::ReadOnly | QIODevice::Unbuffered));

    {
        char buf[5];
        memset(buf, 0, sizeof(buf));
        EXPECT_EQ(smiod.read(buf, 4), 4);
        EXPECT_EQ(QString("LEH!"), buf);
    }
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        EXPECT_EQ(smiod.read(buf, 1023), 2);
        EXPECT_EQ(QString("OL"), buf);
    }
}

TEST(SampleModifyingIODevice, Read_Mocked_Starvation)
{
    using testing::Invoke;
    using testing::Return;
    using testing::_;

    auto inferior = std::make_shared<TestRead>("HELLO!");
    ASSERT_TRUE(inferior->open(QIODevice::ReadOnly));

    SampleModifyingIODevice smiod(inferior, 3, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::ReadOnly | QIODevice::Unbuffered));

    char buf[1024];
    memset(buf, 0, sizeof(buf));

    inferior->setLimit(0); EXPECT_EQ(smiod.read(buf, 1), 0);
    inferior->setLimit(1); EXPECT_EQ(smiod.read(buf, 1), 0);

    inferior->setLimit(0); EXPECT_EQ(smiod.read(buf, 1), 0);
    inferior->setLimit(1); EXPECT_EQ(smiod.read(buf, 1), 0);

    inferior->setLimit(0); EXPECT_EQ(smiod.read(buf, 1), 0);
    inferior->setLimit(1); EXPECT_EQ(smiod.read(buf, 1), 1);
    EXPECT_EQ(QString(buf), "L");

    inferior->setLimit(0); EXPECT_EQ(smiod.read(buf, 1), 1);
    EXPECT_EQ(QString(buf), "E");

    inferior->setLimit(0); EXPECT_EQ(smiod.read(buf, 1), 1);
    EXPECT_EQ(QString(buf), "H");

    inferior->setLimit(0); EXPECT_EQ(smiod.read(buf, 1), 0);

    inferior->setLimit(0); EXPECT_EQ(smiod.read(buf, 1), 0);
    inferior->setLimit(1); EXPECT_EQ(smiod.read(buf, 1), 0);

    inferior->setLimit(0); EXPECT_EQ(smiod.read(buf, 1), 0);
    inferior->setLimit(1); EXPECT_EQ(smiod.read(buf, 1), 0);

    inferior->setLimit(0); EXPECT_EQ(smiod.read(buf, 1), 0);
    inferior->setLimit(1); EXPECT_EQ(smiod.read(buf, 1), 1);
    EXPECT_EQ(QString(buf), "!");

    inferior->setLimit(0); EXPECT_EQ(smiod.read(buf, 1), 1);
    EXPECT_EQ(QString(buf), "O");

    inferior->setLimit(0); EXPECT_EQ(smiod.read(buf, 1), 1);
    EXPECT_EQ(QString(buf), "L");

    inferior->setLimit(0); EXPECT_EQ(smiod.read(buf, 1), 0);
}

TEST(SampleModifyingIODevice, Read_Mocked_ImmediateError)
{
    using testing::Invoke;
    using testing::Return;
    using testing::_;

    auto inferior = std::make_shared<TestRead>("HELLO!");
    ASSERT_TRUE(inferior->open(QIODevice::ReadOnly));

    SampleModifyingIODevice smiod(inferior, 3, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::ReadOnly | QIODevice::Unbuffered));

    char buf[1024];
    memset(buf, 0, sizeof(buf));

    inferior->setError();
    EXPECT_EQ(smiod.read(buf, 1023), -1);

    inferior->unsetError();
    // Right now, the implementation stops when there's been an error, even
    // if the error clears. Preferably, we should retry if the caller retries.

    EXPECT_EQ(smiod.read(buf, 1023), -1);
}

TEST(SampleModifyingIODevice, Read_Mocked_ErrorOnExtraUnit)
{
    using testing::Invoke;
    using testing::Return;
    using testing::_;

    auto inferior = std::make_shared<TestRead>("HELLO!BEN");
    ASSERT_TRUE(inferior->open(QIODevice::ReadOnly));

    SampleModifyingIODevice smiod(inferior, 3, reverseFn);
    ASSERT_TRUE(smiod.open(QIODevice::ReadOnly | QIODevice::Unbuffered));

    char buf[1024];
    memset(buf, 0, sizeof(buf));

    inferior->setErrorAfterLimitReached();
    inferior->setLimit(6);
    EXPECT_EQ(smiod.read(buf, 7), 6);
    EXPECT_EQ(QString(buf), "LEH!OL");

    // The error has already happened, but here is the first place to report it.
    memset(buf, 0, sizeof(buf));
    EXPECT_EQ(smiod.read(buf, 7), -1);
}

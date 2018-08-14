#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "concatiodevice.h"

#include <QBuffer>
#include <QByteArray>

TEST(ConcatIODevice, Simple)
{
    QByteArray hello = QString("Hello").toUtf8();
    QByteArray world = QString("World").toUtf8();

    std::list<QIODevice*> inputs{new QBuffer(&hello), new QBuffer(&world)};

    for (QIODevice* iod : inputs) {
        iod->open(QIODevice::ReadOnly);
    }

    ConcatIODevice ciod(&inputs);
    ciod.open(QIODevice::ReadOnly);

    QString str(ciod.readAll());
    EXPECT_EQ(str, "HelloWorld");
}

TEST(ConcatIODevice, Long)
{
    const int CHUNK_SIZE = 20000; // 16384 is how much Qt seems to read at once.

    QByteArray ones(CHUNK_SIZE, '1');
    QByteArray zeroes(CHUNK_SIZE, '0');

    std::list<QIODevice*> inputs{new QBuffer(&ones), new QBuffer(&zeroes)};

    for (QIODevice* iod : inputs) {
        iod->open(QIODevice::ReadOnly);
    }

    ConcatIODevice ciod(&inputs);
    ciod.open(QIODevice::ReadOnly);

    QByteArray result = ciod.readAll();

    std::vector<char> compareMe(CHUNK_SIZE * 2);
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        compareMe[i] = '1';
    }
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        compareMe[CHUNK_SIZE + i] = '0';
    }

    EXPECT_THAT(result, testing::ElementsAreArray(compareMe));
}

TEST(ConcatIODevice, IsPeekOkay)
{
    QByteArray hello = QString("").toUtf8();
    QByteArray world = QString("World").toUtf8();

    std::list<QIODevice*> inputs{new QBuffer(&hello), new QBuffer(&world)};

    for (QIODevice* iod : inputs) {
        iod->open(QIODevice::ReadOnly);
    }

    ConcatIODevice ciod(&inputs);
    ciod.open(QIODevice::ReadOnly);

    {
        char peekdata{};
        EXPECT_EQ(ciod.peek(&peekdata, 1), 1);
        EXPECT_EQ(peekdata, 'W');
    }

    QString str(ciod.readAll());
    EXPECT_EQ(str, "World");
}

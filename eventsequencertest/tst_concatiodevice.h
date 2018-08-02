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

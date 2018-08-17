#include "tst_samplemodifyingiodevice.h"
#include "tst_channelindex.h"
#include "tst_visualpositionmanager.h"
#include "tst_concatiodevice.h"
#include "tst_testcasename.h"
#include "tst_framesandseconds.h"
#include "tst_collatenonoverlappingsegments.h"

#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

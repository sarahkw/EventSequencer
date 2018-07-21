#include "tst_groupby.h"
#include "tst_merge.h"
#include "tst_mapgenerate.h"
#include "tst_testcasename.h"
#include "tst_strip.h"
#include "tst_framesandseconds.h"
#include "tst_collatenonoverlappingsegments.h"

#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

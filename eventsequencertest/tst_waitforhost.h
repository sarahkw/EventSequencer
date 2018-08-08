#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QObject>

#include "waitforhost.h"

struct WaitForHostTest : testing::Test {
    WaitForHost<int> wfh_;
    std::vector<QObject> qobjs_;
    std::vector<WaitFor*> waitFors_;
    WaitForHostTest() : qobjs_(7)
    {
        for (int i = 0; i < qobjs_.size(); ++i) {
            waitFors_.push_back(wfh_.waitFor(i - 3, &qobjs_[i]));
        }
    }
    std::vector<QObject*> result()
    {
        std::vector<QObject*> result;
        for (WaitFor* wf : waitFors_) {
            result.push_back(wf->result());
        }
        return result;
    }
};

TEST_F(WaitForHostTest, TestTheTest)
{
    EXPECT_THAT(result(),
                testing::ElementsAre(
                    &qobjs_[0], // -3
                    &qobjs_[1],
                    &qobjs_[2],
                    &qobjs_[3], // 0
                    &qobjs_[4],
                    &qobjs_[5],
                    &qobjs_[6]  // 3
            ));
}

TEST_F(WaitForHostTest, Rekey_Pos_AddOne)
{
    wfh_.rekey(2, 1);
    EXPECT_THAT(result(),
                testing::ElementsAre(
                    &qobjs_[0], // -3
                    &qobjs_[1],
                    &qobjs_[2],
                    &qobjs_[3], // 0
                    &qobjs_[4],
                    nullptr,
                    &qobjs_[5]
            ));
}

TEST_F(WaitForHostTest, Rekey_Pos_DelOne)
{
    wfh_.rekey(2, -1);
    EXPECT_THAT(result(),
            testing::ElementsAre(
                &qobjs_[0], // -3
                &qobjs_[1],
                &qobjs_[2],
                &qobjs_[3], // 0
                &qobjs_[4],
                &qobjs_[6],
                nullptr
        ));
}

TEST_F(WaitForHostTest, Rekey_Pos_AddMany)
{
    wfh_.rekey(2, 99);
    EXPECT_THAT(result(),
                testing::ElementsAre(
                    &qobjs_[0], // -3
                    &qobjs_[1],
                    &qobjs_[2],
                    &qobjs_[3], // 0
                    &qobjs_[4],
                    nullptr,
                    nullptr
            ));
}

TEST_F(WaitForHostTest, Rekey_Pos_DelMany)
{
    wfh_.rekey(2, -99);
    EXPECT_THAT(result(),
                testing::ElementsAre(
                    &qobjs_[0], // -3
                    &qobjs_[1],
                    &qobjs_[2],
                    &qobjs_[3], // 0
                    &qobjs_[4],
                    nullptr,
                    nullptr
            ));
}

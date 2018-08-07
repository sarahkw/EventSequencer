#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QObject>

#include "waitforhost.h"

struct WaitForHostTest : testing::Test {
    WaitForHost<int> wfh_;
    std::vector<QObject> qobjs_;
    std::vector<WaitFor*> waitFors_;
    WaitForHostTest() : qobjs_(5)
    {
        WaitForHost<int> wfh;
        for (int i = 0; i < qobjs_.size(); ++i) {
            waitFors_.push_back(wfh.waitFor(i - 2, &qobjs_[i]));
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
                    &qobjs_[0], // -2
                    &qobjs_[1],
                    &qobjs_[2], // 0
                    &qobjs_[3],
                    &qobjs_[4]  // 2
            ));
}

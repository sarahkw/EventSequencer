#include "resourcemetadata.h"

#include <eventsequencer.pb.h>

#include <QtGlobal>

#include <google/protobuf/util/time_util.h>
#include <google/protobuf/util/json_util.h>

using google::protobuf::util::TimeUtil;

std::string ResourceMetaData::createFromNow()
{
    pb::ResourceMetaData rmd;
    *rmd.mutable_createtime() = TimeUtil::GetCurrentTime();
    std::string ret;
    auto result = google::protobuf::util::MessageToJsonString(rmd, &ret);
    if (result.ok()) {
        return ret;
    } else {
        qWarning("Failed greppable 26a2");
        return {};
    }
}

bool ResourceMetaData::read(std::string &input, std::string *createTime)
{
    pb::ResourceMetaData rmd;
    auto result = google::protobuf::util::JsonStringToMessage(input, &rmd);
    if (result.ok()) {
        *createTime = TimeUtil::ToString(rmd.createtime());
        return true;
    } else {
        return false;
    }
}

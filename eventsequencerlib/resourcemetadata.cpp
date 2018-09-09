#include "resourcemetadata.h"

#include <aufileheader.h>
#include <eventsequencer.pb.h>

#include <QtGlobal>
#include <QString>
#include <QFile>

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

bool ResourceMetaData::read(std::string& input, std::string* createTime,
                            qint64* createTimeInSeconds)
{
    pb::ResourceMetaData rmd;
    auto result = google::protobuf::util::JsonStringToMessage(input, &rmd);
    if (result.ok()) {
        if (createTime != nullptr) {
            *createTime = TimeUtil::ToString(rmd.createtime());
        }
        if (createTimeInSeconds != nullptr) {
            *createTimeInSeconds = rmd.createtime().seconds();
        }
        return true;
    } else {
        return false;
    }
}

bool ResourceMetaData::readFromFile(QString filePath, std::string* createTime,
                                    qint64* createTimeInSeconds)
{
    QFile muhFile(filePath);
    if (muhFile.open(QFile::ReadOnly)) {
        AuFileHeader afh;
        std::string metaData;
        if (afh.loadFileAndSeek(muhFile, &metaData)) {
            return read(metaData, createTime, createTimeInSeconds);
        }
    }
    return false;
}

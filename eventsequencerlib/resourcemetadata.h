#ifndef RESOURCEMETADATA_H
#define RESOURCEMETADATA_H

#include <string>

class ResourceMetaData {
    ResourceMetaData() = delete;
public:
    static std::string createFromNow();
    static bool read(std::string& input, std::string* createTime);
};

#endif // RESOURCEMETADATA_H

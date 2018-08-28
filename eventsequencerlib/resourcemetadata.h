#ifndef RESOURCEMETADATA_H
#define RESOURCEMETADATA_H

#include <string>

class ResourceMetaData {
    ResourceMetaData() = delete;
public:
    static std::string createFromNow();
};

#endif // RESOURCEMETADATA_H

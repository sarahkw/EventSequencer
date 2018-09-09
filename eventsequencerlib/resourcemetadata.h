#ifndef RESOURCEMETADATA_H
#define RESOURCEMETADATA_H

#include <QtGlobal>

#include <string>

class ResourceMetaData {
    ResourceMetaData() = delete;
public:
    static std::string createFromNow();
    static bool read(std::string& input,
                     std::string* createTime,
                     qint64* createTimeInSeconds=nullptr);

    // XXX This doesn't really belong here I think.
    static bool readFromFile(QString filePath, std::string* createTime,
                             qint64* createTimeInSeconds = nullptr);
};

#endif // RESOURCEMETADATA_H

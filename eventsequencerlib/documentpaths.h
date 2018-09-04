#ifndef DOCUMENTPATHS_H
#define DOCUMENTPATHS_H

#include <QString>

class DocumentPaths
{
    DocumentPaths() = delete;
public:

    enum class PathRequest {
        DOCUMENT,
        DOCUMENT_BACKUP,
        DATA_DIRECTORY,
        JSON_EXPORT,
        PLAYTOFILE_EXPORT
    };

    // XXX Making this struct because I don't know if QFileInfo requires the
    //     file to actually exist.
    struct PathResponse {
        QString dirPath;
        QString fileName;
        QString filePath;
        bool isDirectory;
    };

    static void pathQuery(QString documentFileName, PathRequest request,
                          PathResponse* response);
};

#endif // DOCUMENTPATHS_H

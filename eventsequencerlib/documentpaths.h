#ifndef DOCUMENTPATHS_H
#define DOCUMENTPATHS_H

#include <QString>
#include <QObject>

// XXX Making this struct because I don't know if QFileInfo requires the
//     file to actually exist.
// I'd like this to be a nested class but it won't work out:
//  Error: Meta object features not supported for nested classes
class DocumentPathsResponse {
    Q_GADGET
public:
    QString dirPath;
    QString fileName;
    QString filePath;
    bool isDirectory;
    Q_PROPERTY(QString dirPath MEMBER dirPath CONSTANT)
    Q_PROPERTY(QString fileName MEMBER fileName CONSTANT)
    Q_PROPERTY(QString filePath MEMBER filePath CONSTANT)
    Q_PROPERTY(bool isDirectory MEMBER isDirectory CONSTANT)
};

Q_DECLARE_METATYPE(DocumentPathsResponse)

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

    static void pathQuery(QString documentFileName, PathRequest request,
                          DocumentPathsResponse* response);
};

#endif // DOCUMENTPATHS_H

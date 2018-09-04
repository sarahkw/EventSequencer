#include "documentpaths.h"

#include "saferfilereplacement.h"

#include <QDir>
#include <QFileInfo>

void DocumentPaths::pathQuery(QString documentFileName,
                              DocumentPaths::PathRequest request,
                              DocumentPathsResponse* response)
{
    QFileInfo fi(documentFileName);
    response->dirPath = fi.dir().path();

    switch (request) {
    case PathRequest::DOCUMENT:
        response->fileName = fi.fileName();
        response->filePath = fi.filePath();
        response->isDirectory = false;
        break;
    case PathRequest::DOCUMENT_BACKUP:
        response->fileName = fi.fileName() + SaferFileReplacement::BACKUP_FILE_SUFFIX;
        response->filePath = fi.filePath() + SaferFileReplacement::BACKUP_FILE_SUFFIX;
        response->isDirectory = false;
        break;
    case PathRequest::DATA_DIRECTORY:
        response->fileName = fi.completeBaseName() + "_data";
        response->filePath = response->dirPath + "/" + response->fileName;
        response->isDirectory = true;
        break;
    case PathRequest::JSON_EXPORT:
        response->fileName = "export-" + fi.completeBaseName() + ".json";
        response->filePath = response->dirPath + "/" + response->fileName;
        response->isDirectory = false;
        break;
    case PathRequest::PLAYTOFILE_EXPORT:
        response->fileName = "export-" + fi.completeBaseName() + ".au";
        response->filePath = response->dirPath + "/" + response->fileName;
        response->isDirectory = false;
        break;
    }
}

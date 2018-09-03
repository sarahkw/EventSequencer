#include "docfillnewdocumentcreator.h"

QString DocFillNewDocumentCreator::documentsPath() const
{
    return documentsPath_;
}

void DocFillNewDocumentCreator::setDocumentsPath(const QString &documentsPath)
{
    if (documentsPath_ != documentsPath) {
        documentsPath_ = documentsPath;
        emit documentsPathChanged();
    }
}

DocFillNewDocumentCreator::DocFillNewDocumentCreator(QObject *parent) : QObject(parent)
{

}

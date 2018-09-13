#include "docfilltextcontenteditor.h"

#include <QDebug>

Document *DocFillTextContentEditor::document() const
{
    return document_;
}

void DocFillTextContentEditor::setDocument(Document *document)
{
    if (document_ != document) {
        if (document_ != nullptr) {
            document_->disconnect(this);
        }
        document_ = document;
        if (document_ != nullptr) {
            QObject::connect(document_, &QObject::destroyed,
                             this, &DocFillTextContentEditor::clearDocument);
        }
        emit documentChanged();
    }
}

void DocFillTextContentEditor::clearDocument()
{
    setDocument(nullptr);
}

DocFillTextContentEditor::DocFillTextContentEditor(QObject *parent) : QObject(parent)
{

}

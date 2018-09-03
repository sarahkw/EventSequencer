#include "docfillnewdocumentcreator.h"

QObject *DocFillNewDocumentCreator::sessionAudio() const
{
    return sessionAudio_;
}

void DocFillNewDocumentCreator::setSessionAudio(QObject *sessionAudio)
{
    SessionAudio* tmp = qobject_cast<SessionAudio*>(sessionAudio);
    if (sessionAudio_ != tmp) {
        if (sessionAudio_ != nullptr) {
            sessionAudio_->disconnect(this);
        }
        if (tmp != nullptr) {
            QObject::connect(tmp, &QObject::destroyed,
                             this, &DocFillNewDocumentCreator::clearSessionAudio);
        }
        sessionAudio_ = tmp;
        emit sessionAudioChanged();
        updateDefaultAudioFormat();
    }
}

void DocFillNewDocumentCreator::clearSessionAudio()
{
    setSessionAudio(nullptr);
}

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

QObject *DocFillNewDocumentCreator::audioFormatHolderQObject()
{
    return document_.audioFormatHolderQObject();
}

QString DocFillNewDocumentCreator::name() const
{
    return name_;
}

void DocFillNewDocumentCreator::setName(const QString &name)
{
    if (name_ != name) {
        name_ = name;
        emit nameChanged();
    }
}

QString DocFillNewDocumentCreator::contents() const
{
    return contents_;
}

void DocFillNewDocumentCreator::setContents(const QString &contents)
{
    if (contents_ != contents) {
        contents_ = contents;
        emit contentsChanged();
    }
}

QVariantList DocFillNewDocumentCreator::make()
{
    return {false, "Not implemented"};
}

void DocFillNewDocumentCreator::updateDefaultAudioFormat()
{
    if (sessionAudio_ != nullptr) {
        sessionAudio_->inputPreferredFormat(document_.audioFormatHolderQObject());
    }
}

DocFillNewDocumentCreator::DocFillNewDocumentCreator(QObject *parent) : QObject(parent)
{
    document_.setAudioFormatHolderSet(true);
}

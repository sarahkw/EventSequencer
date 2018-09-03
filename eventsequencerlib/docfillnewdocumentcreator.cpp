#include "docfillnewdocumentcreator.h"

#include "audioformatholder.h"
#include "channelindex.h"
#include "channel/docfillchannel.h"
#include "channel/textchannel.h"
#include "channel/collatechannel.h"
#include "channel/spanchannel.h"
#include "channel/playlistchannel.h"

#include <QFile>

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
    {
        auto* afh = qobject_cast<AudioFormatHolder*>(document_.audioFormatHolderQObject());
        SessionAudio::TestFormatSupportResult tfsr = sessionAudio_->testFormatSupport(*afh);
        if (!tfsr.allSupported()) {
            return {false, tfsr.describeMissing()};
        }
    }

    if (name_.isEmpty()) {
        // Or else the name will just be ".evseq" which is weird.
        return {false, "Need to provide a name"};
    }

    const auto textChannelIndex = ChannelIndex::make1(1);
    const auto resourceChannelIndex = ChannelIndex::make1(2);
    const auto spanChannelIndex = ChannelIndex::make1(3);

    auto* docFillChannel = qobject_cast<channel::DocFillChannel*>(document_.createChannel(ChannelIndex::make1(0), channel::ChannelType::DocFill));
    auto* textChannel    = qobject_cast<channel::TextChannel*>(document_.createChannel(textChannelIndex, channel::ChannelType::Text));
    auto* collateChannel = qobject_cast<channel::CollateChannel*>(document_.createChannel(resourceChannelIndex, channel::ChannelType::Collate));
    auto* spanChannel    = qobject_cast<channel::SpanChannel*>(document_.createChannel(spanChannelIndex, channel::ChannelType::Span));

    docFillChannel->setTextChannel(textChannelIndex);
    docFillChannel->setResourceChannel(resourceChannelIndex);
    textChannel->setContent(contents());
    collateChannel->setChannel(spanChannelIndex);
    spanChannel->setDefaultChannelType(channel::ChannelType::Playlist);
    spanChannel->setCount(1);

    document_.setEndFrame(contents().size());

    QString filePath = documentsPath_ + "/" + name_ + ".evseq";
    if (QFile::exists(filePath)) {
        // Don't want to save over an existing file.
        return {false, "File already exists"};
    }

    QUrl saveUrl = QUrl::fromLocalFile(filePath);
    return document_.save(saveUrl); // Return the same save method as us!
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

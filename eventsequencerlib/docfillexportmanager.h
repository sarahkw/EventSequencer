#ifndef DOCFILLEXPORTMANAGER_H
#define DOCFILLEXPORTMANAGER_H

#include "channel/channelbase.h"
#include "playable/playablebase.h"

#include <QObject>

class Document;

class DocFillExportManager : public QObject
{
    Q_OBJECT

    Document* document_ = nullptr;
    // Using QObject* just so that Document* can stay an incomplete type.
    Q_PROPERTY(QObject* document READ document WRITE setDocument NOTIFY documentChanged)

    QString defaultExportJsonOutputPath_;
    QString defaultPlayToFileOutputPath_;
    Q_PROPERTY(QString defaultExportJsonOutputPath READ defaultExportJsonOutputPath NOTIFY defaultOutputPathsChanged)
    Q_PROPERTY(QString defaultPlayToFileOutputPath READ defaultPlayToFileOutputPath NOTIFY defaultOutputPathsChanged)

    bool defaultExportJsonOutputPathExists_ = false;
    bool defaultPlayToFileOutputPathExists_ = false;
    Q_PROPERTY(bool defaultExportJsonOutputPathExists READ defaultExportJsonOutputPathExists NOTIFY defaultOutputPathsChanged)
    Q_PROPERTY(bool defaultPlayToFileOutputPathExists READ defaultPlayToFileOutputPathExists NOTIFY defaultOutputPathsChanged)

    void updateDefaultOutputPaths();

public:
    explicit DocFillExportManager(QObject *parent = nullptr);

    QObject *document() const;
    void setDocument(QObject *document);
    void clearDocument();

    QString defaultExportJsonOutputPath() const;

    QString defaultPlayToFileOutputPath() const;

    Q_INVOKABLE QString exportJson(channel::ChannelBase* textChannel,
                                   channel::ChannelBase* resourceChannel);
    Q_INVOKABLE QString exportPlayToFile(playable::PlayableBase* playable);

    bool defaultExportJsonOutputPathExists() const;
    bool defaultPlayToFileOutputPathExists() const;

    Q_INVOKABLE bool deleteDefaultExportJsonOutputPath();
    Q_INVOKABLE bool deleteDefaultPlayToFileOutputPath();

signals:

    void documentChanged();
    void defaultOutputPathsChanged();

public slots:
};

#endif // DOCFILLEXPORTMANAGER_H

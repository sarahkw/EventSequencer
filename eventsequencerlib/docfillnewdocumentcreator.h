#ifndef DOCFILLNEWDOCUMENTCREATOR_H
#define DOCFILLNEWDOCUMENTCREATOR_H

#include "document.h"
#include "sessionaudio.h"

#include <QVariantList>
#include <QObject>

class DocFillNewDocumentCreator : public QObject
{
    Q_OBJECT

    SessionAudio* sessionAudio_ = nullptr;
    QString documentsPath_;
    Q_PROPERTY(QObject* sessionAudio READ sessionAudio WRITE setSessionAudio NOTIFY sessionAudioChanged)
    Q_PROPERTY(QString documentsPath READ documentsPath WRITE setDocumentsPath NOTIFY documentsPathChanged)

    Document document_;

    QString name_;
    QString contents_;
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString contents READ contents WRITE setContents NOTIFY contentsChanged)
    Q_PROPERTY(QObject* audioFormatHolder READ audioFormatHolderQObject CONSTANT)

    void updateDefaultAudioFormat();

public:
    explicit DocFillNewDocumentCreator(QObject *parent = nullptr);

    QObject *sessionAudio() const;
    void setSessionAudio(QObject *sessionAudio);
    void clearSessionAudio();

    QString documentsPath() const;
    void setDocumentsPath(const QString &documentsPath);

    QObject* audioFormatHolderQObject();

    QString name() const;
    void setName(const QString &name);

    QString contents() const;
    void setContents(const QString &contents);

    Q_INVOKABLE QVariantList make();

signals:

    void sessionAudioChanged();
    void documentsPathChanged();
    void nameChanged();
    void contentsChanged();

public slots:
};

#endif // DOCFILLNEWDOCUMENTCREATOR_H

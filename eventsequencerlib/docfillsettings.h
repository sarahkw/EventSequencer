#ifndef DOCFILLSETTINGS_H
#define DOCFILLSETTINGS_H

#include <QObject>
#include <QSettings>

class DocFillSettings : public QObject
{
    Q_OBJECT

    QSettings settings_;

    QString defaultDocumentsPath_;
    QString documentsPath_;
    Q_PROPERTY(QString defaultDocumentsPath READ defaultDocumentsPath CONSTANT)
    Q_PROPERTY(QString documentsPath READ documentsPath WRITE setDocumentsPath NOTIFY documentsPathChanged)
    Q_PROPERTY(bool documentsPathIsDefault READ documentsPathIsDefault NOTIFY documentsPathChanged)

    int fontPointSize_ = 0;
    Q_PROPERTY(int fontPointSize READ fontPointSize WRITE setFontPointSize NOTIFY fontPointSizeChanged)

    int toneFrequency_ = 0;
    bool reviewAfterRecord_ = false;
    bool selectOnRecord_ = false;
    bool toneBeforeRecord_ = false;
    bool developerOptions_ = false;
    bool updateStatistics_ = true;
    Q_PROPERTY(int  toneFrequency     READ toneFrequency     WRITE setToneFrequency     NOTIFY cheapUserSettingsChanged)
    Q_PROPERTY(bool reviewAfterRecord READ reviewAfterRecord WRITE setReviewAfterRecord NOTIFY cheapUserSettingsChanged)
    Q_PROPERTY(bool selectOnRecord    READ selectOnRecord    WRITE setSelectOnRecord    NOTIFY cheapUserSettingsChanged)
    Q_PROPERTY(bool toneBeforeRecord  READ toneBeforeRecord  WRITE setToneBeforeRecord  NOTIFY cheapUserSettingsChanged)
    Q_PROPERTY(bool developerOptions  READ developerOptions  WRITE setDeveloperOptions  NOTIFY cheapUserSettingsChanged)
    Q_PROPERTY(bool updateStatistics  READ updateStatistics  WRITE setUpdateStatistics  NOTIFY cheapUserSettingsChanged)

public:
    explicit DocFillSettings(QObject *parent = nullptr);

    QString defaultDocumentsPath() const;

    QString documentsPath() const;
    void setDocumentsPath(const QString &documentsPath);
    Q_INVOKABLE void unsetDocumentsPath();
    bool documentsPathIsDefault() const;

    int fontPointSize() const;
    void setFontPointSize(int fontPointSize);

    int toneFrequency() const;
    void setToneFrequency(int toneFrequency);

    bool reviewAfterRecord() const;
    void setReviewAfterRecord(bool reviewAfterRecord);

    bool selectOnRecord() const;
    void setSelectOnRecord(bool selectOnRecord);

    bool toneBeforeRecord() const;
    void setToneBeforeRecord(bool toneBeforeRecord);

    bool developerOptions() const;
    void setDeveloperOptions(bool developerOptions);

    bool updateStatistics() const;
    void setUpdateStatistics(bool updateStatistics);

signals:

    void documentsPathChanged();

    void fontPointSizeChanged();
    // Font point size isn't cheap because if it ends up changing the font and
    // relaying everything out that'd be expensive. I'm guessing it probably
    // won't trigger the relayout unless the number did actually change, but
    // why spend the time verifying that when I could make another property?
    void cheapUserSettingsChanged();

public slots:
};

#endif // DOCFILLSETTINGS_H

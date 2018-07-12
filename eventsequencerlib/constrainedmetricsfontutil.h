#ifndef MUHFONT_H
#define MUHFONT_H

#include <QObject>
#include <QFont>

class ConstrainedMetricsFontUtil : public QObject
{
    Q_OBJECT

    int constrainByWidthValue_ = 0;

    bool constainByHeightEnabled_ = false;
    int constrainByHeightValue_ = 0;

    bool addLetterSpacingToMatchWidth_ = false;

    QFont baseFont_;

    bool builtFontFailedToMeetConstraints_ = false;
    int builtFontAddedSpacing_ = 0;

    Q_PROPERTY(int   constrainByWidthValue            READ constrainByWidthValue            WRITE setConstrainByWidthValue            NOTIFY constrainByWidthValueChanged)
    Q_PROPERTY(bool  constainByHeightEnabled          READ constainByHeightEnabled          WRITE setConstainByHeightEnabled          NOTIFY constainByHeightEnabledChanged)
    Q_PROPERTY(int   constrainByHeightValue           READ constrainByHeightValue           WRITE setConstrainByHeightValue           NOTIFY constrainByHeightValueChanged)
    Q_PROPERTY(bool  addLetterSpacingToMatchWidth     READ addLetterSpacingToMatchWidth     WRITE setAddLetterSpacingToMatchWidth     NOTIFY addLetterSpacingToMatchWidthChanged)
    Q_PROPERTY(QFont baseFont                         READ baseFont                         WRITE setBaseFont                         NOTIFY baseFontChanged)
    Q_PROPERTY(bool  builtFontFailedToMeetConstraints READ builtFontFailedToMeetConstraints   NOTIFY builtFontFailedToMeetConstraintsChanged)
    Q_PROPERTY(int   builtFontAddedSpacing            READ builtFontAddedSpacing              NOTIFY builtFontAddedSpacingChanged)

public:
    explicit ConstrainedMetricsFontUtil(QObject *parent = nullptr);

    Q_INVOKABLE static QFont defaultFont();
    Q_INVOKABLE static int fontCharacterWidth(QFont font);
    Q_INVOKABLE static int fontHeight(QFont font);
    Q_INVOKABLE static QFont makeUniformPixelWidth(QFont input);
    Q_INVOKABLE static QString fontBriefInformation(QFont font);

    int constrainByWidthValue() const;
    void setConstrainByWidthValue(int constrainByWidthValue);

    bool constainByHeightEnabled() const;
    void setConstainByHeightEnabled(bool constainByHeightEnabled);

    int constrainByHeightValue() const;
    void setConstrainByHeightValue(int constrainByHeightValue);

    bool addLetterSpacingToMatchWidth() const;
    void setAddLetterSpacingToMatchWidth(bool addLetterSpacingToMatchWidth);

    QFont baseFont() const;
    void setBaseFont(const QFont &baseFont);

    Q_INVOKABLE QFont buildFont();

    bool builtFontFailedToMeetConstraints() const;
private:
    void setBuiltFontFailedToMeetConstraints(bool builtFontFailedToMeetConstraints);
public:

    int builtFontAddedSpacing() const;
private:
    void setBuiltFontAddedSpacing(int builtFontAddedSpacing);
public:

signals:

    void constrainByWidthValueChanged();
    void constainByHeightEnabledChanged();
    void constrainByHeightValueChanged();
    void addLetterSpacingToMatchWidthChanged();
    void baseFontChanged();
    void builtFontFailedToMeetConstraintsChanged();
    void builtFontAddedSpacingChanged();

public slots:
};

#endif // MUHFONT_H

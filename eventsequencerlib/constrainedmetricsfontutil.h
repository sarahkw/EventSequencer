#ifndef CONSTRAINEDMETRICSFONTUTIL_H
#define CONSTRAINEDMETRICSFONTUTIL_H

#include <QObject>
#include <QFont>

class ConstrainedMetricsFontUtil : public QObject
{
    Q_OBJECT

    bool constrainByWidthEnabled_ = true; // Allowing this to be false was added later.
    int constrainByWidthValue_ = 0;

    bool constrainByHeightEnabled_ = false;
    int constrainByHeightValue_ = 0;

    bool addLetterSpacingToMatchWidth_ = false;

    QFont baseFont_;

    bool builtFontFailedToMeetConstraints_ = false;
    int builtFontAddedSpacing_ = 0;
    int builtFontHeight_ = 0;
    int builtFontWidth_ = 0;

    QFont builtFont_;

    Q_PROPERTY(bool  constrainByWidthEnabled          READ constrainByWidthEnabled          WRITE setConstrainByWidthEnabled          NOTIFY constrainByWidthEnabledChanged)
    Q_PROPERTY(int   constrainByWidthValue            READ constrainByWidthValue            WRITE setConstrainByWidthValue            NOTIFY constrainByWidthValueChanged)
    Q_PROPERTY(bool  constrainByHeightEnabled         READ constrainByHeightEnabled         WRITE setconstrainByHeightEnabled         NOTIFY constrainByHeightEnabledChanged)
    Q_PROPERTY(int   constrainByHeightValue           READ constrainByHeightValue           WRITE setConstrainByHeightValue           NOTIFY constrainByHeightValueChanged)
    Q_PROPERTY(bool  addLetterSpacingToMatchWidth     READ addLetterSpacingToMatchWidth     WRITE setAddLetterSpacingToMatchWidth     NOTIFY addLetterSpacingToMatchWidthChanged)
    Q_PROPERTY(QFont baseFont                         READ baseFont                         WRITE setBaseFont                         NOTIFY baseFontChanged)
    Q_PROPERTY(bool  builtFontFailedToMeetConstraints READ builtFontFailedToMeetConstraints   NOTIFY builtFontFailedToMeetConstraintsChanged)
    Q_PROPERTY(int   builtFontAddedSpacing            READ builtFontAddedSpacing              NOTIFY builtFontAddedSpacingChanged)
    Q_PROPERTY(int   builtFontHeight                  READ builtFontHeight                    NOTIFY builtFontHeightChanged)
    Q_PROPERTY(int   builtFontWidth                   READ builtFontWidth                     NOTIFY builtFontWidthChanged)
    Q_PROPERTY(QFont builtFont                        READ builtFont                          NOTIFY builtFontChanged)

    bool builtFontIsDirty_ = false;

public:
    explicit ConstrainedMetricsFontUtil(QObject *parent = nullptr);

    Q_INVOKABLE static QFont defaultFont();
    Q_INVOKABLE static int fontCharacterWidth(QFont font);
    Q_INVOKABLE static int fontHeight(QFont font);
    Q_INVOKABLE static QFont makeUniformPixelWidth(QFont input);

    bool constrainByWidthEnabled() const;
    void setConstrainByWidthEnabled(bool constrainByWidthEnabled);

    int constrainByWidthValue() const;
    void setConstrainByWidthValue(int constrainByWidthValue);

    bool constrainByHeightEnabled() const;
    void setconstrainByHeightEnabled(bool constrainByHeightEnabled);

    int constrainByHeightValue() const;
    void setConstrainByHeightValue(int constrainByHeightValue);

    bool addLetterSpacingToMatchWidth() const;
    void setAddLetterSpacingToMatchWidth(bool addLetterSpacingToMatchWidth);

    QFont baseFont() const;
    void setBaseFont(const QFont &baseFont);

private:
    QFont buildFont();
public:

    bool builtFontFailedToMeetConstraints() const;
private:
    void setBuiltFontFailedToMeetConstraints(bool builtFontFailedToMeetConstraints);
public:

    int builtFontAddedSpacing() const;
private:
    void setBuiltFontAddedSpacing(int builtFontAddedSpacing);
public:

    int builtFontHeight() const;
private:
    void setBuiltFontHeight(int builtFontHeight);
public:

    int builtFontWidth() const;
private:
    void setBuiltFontWidth(int builtFontWidth);
public:

    QFont builtFont();

signals:

    void constrainByWidthEnabledChanged();
    void constrainByWidthValueChanged();
    void constrainByHeightEnabledChanged();
    void constrainByHeightValueChanged();
    void addLetterSpacingToMatchWidthChanged();
    void baseFontChanged();
    void builtFontFailedToMeetConstraintsChanged();
    void builtFontAddedSpacingChanged();
    void builtFontHeightChanged();
    void builtFontWidthChanged();
    void builtFontChanged();

public slots:
};

#endif // CONSTRAINEDMETRICSFONTUTIL_H

#include "constrainedmetricsfontutil.h"

#include <QFontMetrics>
#include <QFontDatabase>

bool ConstrainedMetricsFontUtil::constrainByWidthEnabled() const
{
    return constrainByWidthEnabled_;
}

void ConstrainedMetricsFontUtil::setConstrainByWidthEnabled(bool constrainByWidthEnabled)
{
    if (constrainByWidthEnabled_ != constrainByWidthEnabled) {
        constrainByWidthEnabled_ = constrainByWidthEnabled;
        emit constrainByWidthEnabledChanged();

        builtFontIsDirty_ = true;
        emit builtFontChanged();
    }
}

int ConstrainedMetricsFontUtil::constrainByWidthValue() const
{
    return constrainByWidthValue_;
}

void ConstrainedMetricsFontUtil::setConstrainByWidthValue(int constrainByWidthValue)
{
    if (constrainByWidthValue_ != constrainByWidthValue) {
        constrainByWidthValue_ = constrainByWidthValue;
        emit constrainByWidthValueChanged();

        builtFontIsDirty_ = true;
        emit builtFontChanged();
    }
}

bool ConstrainedMetricsFontUtil::constrainByHeightEnabled() const
{
    return constrainByHeightEnabled_;
}

void ConstrainedMetricsFontUtil::setconstrainByHeightEnabled(bool constrainByHeightEnabled)
{
    if (constrainByHeightEnabled_ != constrainByHeightEnabled) {
        constrainByHeightEnabled_ = constrainByHeightEnabled;
        emit constrainByHeightEnabledChanged();

        builtFontIsDirty_ = true;
        emit builtFontChanged();
    }
}

int ConstrainedMetricsFontUtil::constrainByHeightValue() const
{
    return constrainByHeightValue_;
}

void ConstrainedMetricsFontUtil::setConstrainByHeightValue(int constrainByHeightValue)
{
    if (constrainByHeightValue_ != constrainByHeightValue) {
        constrainByHeightValue_ = constrainByHeightValue;
        emit constrainByHeightValueChanged();

        builtFontIsDirty_ = true;
        emit builtFontChanged();
    }
}

bool ConstrainedMetricsFontUtil::addLetterSpacingToMatchWidth() const
{
    return addLetterSpacingToMatchWidth_;
}

void ConstrainedMetricsFontUtil::setAddLetterSpacingToMatchWidth(bool addLetterSpacingToMatchWidth)
{
    if (addLetterSpacingToMatchWidth_ != addLetterSpacingToMatchWidth) {
        addLetterSpacingToMatchWidth_ = addLetterSpacingToMatchWidth;
        emit addLetterSpacingToMatchWidthChanged();

        builtFontIsDirty_ = true;
        emit builtFontChanged();
    }
}

QFont ConstrainedMetricsFontUtil::baseFont() const
{
    return baseFont_;
}

void ConstrainedMetricsFontUtil::setBaseFont(const QFont &baseFont)
{
    if (baseFont_ != baseFont) {
        baseFont_ = makeUniformPixelWidth(baseFont); // Bad stuff happens when not set.
        emit baseFontChanged();

        builtFontIsDirty_ = true;
        emit builtFontChanged();
    }
}

QFont ConstrainedMetricsFontUtil::buildFont()
{
    QFont candidate = baseFont_;
    bool gotGoodCandidate = false;

    if (constrainByWidthEnabled()) {
        const int MIN_POINT_SIZE = 1;
        const int MAX_POINT_SIZE = 100;
        candidate.setPointSize(MIN_POINT_SIZE);
        candidate.setLetterSpacing(QFont::AbsoluteSpacing, 0);
        int goodCandidateMissingWidth = -1;
        int noCandidateMissingWidth = -1;

        for (int i = MIN_POINT_SIZE; i < MAX_POINT_SIZE; ++i) {
            QFont experimental = candidate;
            experimental.setPointSize(i);
            QFontMetrics fm(experimental);
            const int charWidth = fm.horizontalAdvance("x");
            noCandidateMissingWidth = constrainByWidthValue() - charWidth;
            if (charWidth > constrainByWidthValue()) {
                break;
            }
            if (constrainByHeightEnabled() && fm.height() > constrainByHeightValue()) {
                break;
            }
            candidate = experimental;
            gotGoodCandidate = true;
            goodCandidateMissingWidth = noCandidateMissingWidth;
        }

        if (addLetterSpacingToMatchWidth()) {
            int letterSpacing = gotGoodCandidate ? goodCandidateMissingWidth : noCandidateMissingWidth;
            candidate.setLetterSpacing(QFont::AbsoluteSpacing, letterSpacing);
            setBuiltFontAddedSpacing(letterSpacing);
        } else {
            candidate.setLetterSpacing(QFont::AbsoluteSpacing, 0);
            setBuiltFontAddedSpacing(0);
        }
    } else {
        gotGoodCandidate = true;
        candidate.setLetterSpacing(QFont::AbsoluteSpacing, 0);
        setBuiltFontAddedSpacing(0);
    }

    setBuiltFontFailedToMeetConstraints(!gotGoodCandidate);

    QFontMetrics fm(candidate); // XXX I hope this is fast.
    setBuiltFontHeight(fm.height());
    setBuiltFontWidth(fm.horizontalAdvance("x"));

    return candidate;
}

bool ConstrainedMetricsFontUtil::builtFontFailedToMeetConstraints() const
{
    return builtFontFailedToMeetConstraints_;
}

void ConstrainedMetricsFontUtil::setBuiltFontFailedToMeetConstraints(bool builtFontFailedToMeetConstraints)
{
    if (builtFontFailedToMeetConstraints_ != builtFontFailedToMeetConstraints) {
        builtFontFailedToMeetConstraints_ = builtFontFailedToMeetConstraints;
        emit builtFontFailedToMeetConstraintsChanged();
    }
}

int ConstrainedMetricsFontUtil::builtFontAddedSpacing() const
{
    return builtFontAddedSpacing_;
}

void ConstrainedMetricsFontUtil::setBuiltFontAddedSpacing(int builtFontAddedSpacing)
{
    if (builtFontAddedSpacing_ != builtFontAddedSpacing) {
        builtFontAddedSpacing_ = builtFontAddedSpacing;
        emit builtFontAddedSpacingChanged();
    }
}

int ConstrainedMetricsFontUtil::builtFontHeight() const
{
    return builtFontHeight_;
}

void ConstrainedMetricsFontUtil::setBuiltFontHeight(int builtFontHeight)
{
    if (builtFontHeight_ != builtFontHeight) {
        builtFontHeight_ = builtFontHeight;
        emit builtFontHeightChanged();
    }
}

int ConstrainedMetricsFontUtil::builtFontWidth() const
{
    return builtFontWidth_;
}

void ConstrainedMetricsFontUtil::setBuiltFontWidth(int builtFontWidth)
{
    if (builtFontWidth_ != builtFontWidth) {
        builtFontWidth_ = builtFontWidth;
        emit builtFontWidthChanged();
    }
}

QFont ConstrainedMetricsFontUtil::builtFont()
{
    if (builtFontIsDirty_) {
        // XXX After profiling, perhaps find a way to reduce buildFont
        //     calls during initialization. Perhaps encapsulate all
        //     the parameters in a structure that has dependencies on
        //     all the necessary params.
        //
        // qInfo() << "builtFont";
        builtFont_ = buildFont();
        builtFontIsDirty_ = false;
    }
    return builtFont_;
}

ConstrainedMetricsFontUtil::ConstrainedMetricsFontUtil(QObject *parent) : QObject(parent)
{
    baseFont_ = makeUniformPixelWidth(defaultFont());
}

QFont ConstrainedMetricsFontUtil::defaultFont()
{
#ifdef Q_OS_ANDROID
    // I've observed that Android doesn't get a default fixed font. Unfortunately,
    // hardcode an Android font.
    return {"Droid Sans Mono"};
#else
    return QFontDatabase::systemFont(QFontDatabase::FixedFont);
#endif
}

int ConstrainedMetricsFontUtil::fontCharacterWidth(QFont font)
{
    QFontMetrics fm(font);
    return fm.horizontalAdvance("x");
}

int ConstrainedMetricsFontUtil::fontHeight(QFont font)
{
    QFontMetrics fm(font);
    return fm.height();
}

QFont ConstrainedMetricsFontUtil::makeUniformPixelWidth(QFont input)
{
    QFont copy(input);
    copy.setStyleStrategy(static_cast<QFont::StyleStrategy>(copy.styleStrategy() | QFont::ForceIntegerMetrics));
    copy.setKerning(false);
    return copy;
}

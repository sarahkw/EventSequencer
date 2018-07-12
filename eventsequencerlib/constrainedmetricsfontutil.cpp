#include "constrainedmetricsfontutil.h"

#include <QDebug>
#include <QFontMetrics>
#include <QFontDatabase>
#include <QFontInfo>

int ConstrainedMetricsFontUtil::constrainByWidthValue() const
{
    return constrainByWidthValue_;
}

void ConstrainedMetricsFontUtil::setConstrainByWidthValue(int constrainByWidthValue)
{
    if (constrainByWidthValue_ != constrainByWidthValue) {
        constrainByWidthValue_ = constrainByWidthValue;
        emit constrainByWidthValueChanged();
    }
}

bool ConstrainedMetricsFontUtil::constainByHeightEnabled() const
{
    return constainByHeightEnabled_;
}

void ConstrainedMetricsFontUtil::setConstainByHeightEnabled(bool constainByHeightEnabled)
{
    if (constainByHeightEnabled_ != constainByHeightEnabled) {
        constainByHeightEnabled_ = constainByHeightEnabled;
        emit constainByHeightEnabledChanged();
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
    }
}

QFont ConstrainedMetricsFontUtil::baseFont() const
{
    return baseFont_;
}

void ConstrainedMetricsFontUtil::setBaseFont(const QFont &baseFont)
{
    if (baseFont_ != baseFont) {
        baseFont_ = baseFont;
        emit baseFontChanged();
    }
}

QFont ConstrainedMetricsFontUtil::buildFont()
{
    const int MIN_POINT_SIZE = 1;
    const int MAX_POINT_SIZE = 100;
    QFont candidate = baseFont_;
    candidate.setPointSize(MIN_POINT_SIZE);
    candidate.setLetterSpacing(QFont::AbsoluteSpacing, 0);
    bool gotGoodCandidate = false;
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
        if (constainByHeightEnabled() && fm.height() > constrainByHeightValue()) {
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
    }

    setBuiltFontFailedToMeetConstraints(!gotGoodCandidate);
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

ConstrainedMetricsFontUtil::ConstrainedMetricsFontUtil(QObject *parent) : QObject(parent)
{
    baseFont_ = makeUniformPixelWidth(defaultFont());
}

QFont ConstrainedMetricsFontUtil::defaultFont()
{
    return QFontDatabase::systemFont(QFontDatabase::FixedFont);
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

QString ConstrainedMetricsFontUtil::fontBriefInformation(QFont font)
{
    return QString("k = %1 sS = %2 cW = %3")
            .arg(font.kerning()).arg(font.styleStrategy()).arg(fontCharacterWidth(font));
}

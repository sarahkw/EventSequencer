#include "constrainedmetricsfontutil.h"

#include <QDebug>
#include <QFontMetrics>
#include <QFontDatabase>

ConstrainedMetricsFontUtil::ConstrainedMetricsFontUtil(QObject *parent) : QObject(parent)
{
}

QFont ConstrainedMetricsFontUtil::defaultFont() const
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

void ConstrainedMetricsFontUtil::dumpFontInformation(QFont font)
{
    QFontMetrics fm(font);
    qInfo() <<

"ascent()           = " << fm.ascent()           << "\n" <<
"capHeight()        = " << fm.capHeight()        << "\n" <<
"descent()          = " << fm.descent()          << "\n" <<
"height()           = " << fm.height()           << "\n" <<
"leading()          = " << fm.leading()          << "\n" <<
"lineSpacing()      = " << fm.lineSpacing()      << "\n" <<
"minLeftBearing()   = " << fm.minLeftBearing()   << "\n" <<
"minRightBearing()  = " << fm.minRightBearing()  << "\n" <<
"maxWidth()         = " << fm.maxWidth()         << "\n" <<
                                                    "\n" <<
"xHeight()          = " << fm.xHeight()          << "\n" <<
"averageCharWidth() = " << fm.averageCharWidth() << "\n" <<
                                                    "\n" <<
"underlinePos()     = " << fm.underlinePos()     << "\n" <<
"overlinePos()      = " << fm.overlinePos()      << "\n" <<
"strikeOutPos()     = " << fm.strikeOutPos()     << "\n" <<
"lineWidth()        = " << fm.lineWidth()        << "\n" <<

"";
}

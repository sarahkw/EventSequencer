#include "constrainedmetricsfontutil.h"

#include <QDebug>
#include <QFontMetrics>


ConstrainedMetricsFontUtil::ConstrainedMetricsFontUtil(QObject *parent) : QObject(parent)
{
}

QFont ConstrainedMetricsFontUtil::makeFont(int pixelSize, qreal spacing)
{
    QFont ret("Courier New");
    ret.setStyleStrategy(static_cast<QFont::StyleStrategy>(QFont::PreferDefault | QFont::ForceIntegerMetrics));
    ret.setPixelSize(pixelSize);
    ret.setKerning(false);
    ret.setLetterSpacing(QFont::AbsoluteSpacing, spacing);

    return ret;
}

int ConstrainedMetricsFontUtil::fontCharacterWidth(int pixelSize, qreal spacing)
{
    QFont f = makeFont(pixelSize, spacing);
    QFontMetrics fm(f);
    return fm.horizontalAdvance("x");
}

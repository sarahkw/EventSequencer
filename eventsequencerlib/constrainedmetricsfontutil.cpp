#include "constrainedmetricsfontutil.h"

#include <QDebug>
#include <QFontMetrics>


ConstrainedMetricsFontUtil::ConstrainedMetricsFontUtil(QObject *parent) : QObject(parent)
{
}

QFont ConstrainedMetricsFontUtil::defaultFont() const
{
    return QFont("Courier New");
}

int ConstrainedMetricsFontUtil::fontCharacterWidth(QFont font)
{
    QFontMetrics fm(font);
    return fm.horizontalAdvance("x");
}

QFont ConstrainedMetricsFontUtil::makeUniformPixelWidth(QFont input)
{
    QFont copy(input);
    copy.setStyleStrategy(static_cast<QFont::StyleStrategy>(copy.styleStrategy() | QFont::ForceIntegerMetrics));
    copy.setKerning(false);
    return copy;
}

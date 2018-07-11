#ifndef MUHFONT_H
#define MUHFONT_H

#include <QObject>
#include <QFont>

class ConstrainedMetricsFontUtil : public QObject
{
    Q_OBJECT

public:
    explicit ConstrainedMetricsFontUtil(QObject *parent = nullptr);

    Q_INVOKABLE QFont makeFont(int pixelSize, qreal spacing);
    Q_INVOKABLE int fontCharacterWidth(int pixelSize, qreal spacing);

signals:

public slots:
};

#endif // MUHFONT_H

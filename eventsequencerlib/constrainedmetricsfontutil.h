#ifndef MUHFONT_H
#define MUHFONT_H

#include <QObject>
#include <QFont>

class ConstrainedMetricsFontUtil : public QObject
{
    Q_OBJECT

public:
    explicit ConstrainedMetricsFontUtil(QObject *parent = nullptr);

    Q_INVOKABLE QFont defaultFont() const;
    Q_INVOKABLE int fontCharacterWidth(QFont font);
    Q_INVOKABLE int fontHeight(QFont font);
    Q_INVOKABLE QFont makeUniformPixelWidth(QFont input);
    Q_INVOKABLE void dumpFontInformation(QFont font);

signals:

public slots:
};

#endif // MUHFONT_H

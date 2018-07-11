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
    Q_INVOKABLE QFont makeUniformPixelWidth(QFont input);

signals:

public slots:
};

#endif // MUHFONT_H

#ifndef DOCFILLBACKFILLSTATS_H
#define DOCFILLBACKFILLSTATS_H

#include <QObject>

class DocFillBackfillStats : public QObject
{
    Q_OBJECT
public:
    explicit DocFillBackfillStats(QObject *parent = nullptr);

signals:

public slots:
};

#endif // DOCFILLBACKFILLSTATS_H
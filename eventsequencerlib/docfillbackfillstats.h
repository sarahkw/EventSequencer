#ifndef DOCFILLBACKFILLSTATS_H
#define DOCFILLBACKFILLSTATS_H

#include "document.h"
#include "docfilldatabase.h"

#include <QObject>

#include <map>

class DocFillBackfillStats : public QObject
{
    Q_OBJECT

    Document document_;

    DocFillDatabase* docfillDatabase_ = nullptr;
    QString filePath_;
    QString report_;
    Q_PROPERTY(DocFillDatabase* docfillDatabase READ docfillDatabase WRITE setDocfillDatabase NOTIFY docfillDatabaseChanged)
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(QString report READ report NOTIFY reportChanged)

    std::map<int, qint64> rawData_;

    void updateReport();

public:
    explicit DocFillBackfillStats(QObject *parent = nullptr);

    Q_INVOKABLE QString actuallyBackfill();

    DocFillDatabase *docfillDatabase() const;
    void setDocfillDatabase(DocFillDatabase *docfillDatabase);
    void clearDocfillDatabase();

    QString filePath() const;
    void setFilePath(const QString &filePath);

    QString report() const;

signals:

    void docfillDatabaseChanged();
    void filePathChanged();
    void reportChanged();

public slots:
};

#endif // DOCFILLBACKFILLSTATS_H

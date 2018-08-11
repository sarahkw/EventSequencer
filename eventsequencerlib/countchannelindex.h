#ifndef COUNTCHANNELINDEX_H
#define COUNTCHANNELINDEX_H

#include <QObject>
#include <QVariantList>

class CountChannelIndex : public QObject
{
    Q_OBJECT

    QObject* document_ = nullptr;
    int startAtPosition_ = 0;
    int count_ = 0;

    QVariantList model_;

    Q_PROPERTY(QObject*      document        READ document        WRITE setDocument        NOTIFY documentChanged)
    Q_PROPERTY(int           startAtPosition READ startAtPosition WRITE setStartAtPosition NOTIFY startAtPositionChanged)
    Q_PROPERTY(int           count           READ count           WRITE setCount           NOTIFY countChanged)
    Q_PROPERTY(QVariantList  model           READ model                                    NOTIFY modelChanged)
    
    void updateModel();

public:
    CountChannelIndex(QObject* parent = nullptr);

    QObject *document() const;
    void setDocument(QObject *document);
    void documentDestroyed();

    int startAtPosition() const;
    void setStartAtPosition(int startAtPosition);

    int count() const;
    void setCount(int count);

    QVariantList model() const;

signals:

    void documentChanged();
    void startAtPositionChanged();
    void countChanged();

    void modelChanged();
};

#endif // COUNTCHANNELINDEX_H

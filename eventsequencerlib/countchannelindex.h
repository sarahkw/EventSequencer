#ifndef COUNTCHANNELINDEX_H
#define COUNTCHANNELINDEX_H

#include "channelindex.h"

#include <QObject>
#include <QAbstractListModel>

class CountChannelIndex : public QAbstractListModel
{
    Q_OBJECT

    QObject* document_ = nullptr;
    int startAtPosition_ = 0;
    int count_ = 0;

    Q_PROPERTY(QObject*      document        READ document        WRITE setDocument        NOTIFY documentChanged)
    Q_PROPERTY(int           startAtPosition READ startAtPosition WRITE setStartAtPosition NOTIFY startAtPositionChanged)
    Q_PROPERTY(int           count           READ count           WRITE setCount           NOTIFY countChanged)
    
    std::vector<ChannelIndex> current_;

    void updateModel();

    enum CustomRoles {
        ModelDataRole = Qt::UserRole + 1
    };

public:
    CountChannelIndex(QObject* parent = nullptr);

    QObject *document() const;
    void setDocument(QObject *document);
    void documentDestroyed();

    int startAtPosition() const;
    void setStartAtPosition(int startAtPosition);

    int count() const;
    void setCount(int count);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int,QByteArray> roleNames() const override;

signals:

    void documentChanged();
    void startAtPositionChanged();
    void countChanged();
};

#endif // COUNTCHANNELINDEX_H

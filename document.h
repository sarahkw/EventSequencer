#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QAbstractListModel>

class Strip;

class Document : public QAbstractListModel
{
    Q_OBJECT

    enum CustomRoles {
        ModelDataRole = Qt::UserRole + 1
    };

    std::vector<Strip*> strips_;

public:
    explicit Document(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int,QByteArray> roleNames() const override;

    Q_INVOKABLE Strip* createStrip();
    Q_INVOKABLE void deleteStrip(Strip* strip);

signals:

public slots:
};

#endif // DOCUMENT_H

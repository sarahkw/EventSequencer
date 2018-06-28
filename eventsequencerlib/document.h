#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QAbstractListModel>

namespace pb {
class Document;
}

class Strip;

class Document : public QAbstractListModel
{
    Q_OBJECT

    enum CustomRoles {
        ModelDataRole = Qt::UserRole + 1
    };

    std::vector<Strip*> strips_;

    int framesPerSecond_;

    Q_PROPERTY(int framesPerSecond READ framesPerSecond WRITE setFramesPerSecond NOTIFY framesPerSecondChanged)

public:
    explicit Document(QObject *parent = nullptr);

    void toPb(pb::Document& pb) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int,QByteArray> roleNames() const override;

    Q_INVOKABLE Strip* createStrip();
    Q_INVOKABLE void deleteStrip(Strip* strip);
    Q_INVOKABLE QVariantList strips(); // QVariantList for QML
    Q_INVOKABLE void save(const QString& fileName);

    int framesPerSecond() const;
    void setFramesPerSecond(int framesPerSecond);

signals:

    void framesPerSecondChanged();

public slots:
};

#endif // DOCUMENT_H

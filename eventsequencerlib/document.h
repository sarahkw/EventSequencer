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

    int framesPerSecond_ = 30;
    int startFrame_ = 0;
    int endFrame_ = 250;

    Q_PROPERTY(int framesPerSecond READ framesPerSecond WRITE setFramesPerSecond NOTIFY framesPerSecondChanged)
    Q_PROPERTY(int startFrame READ startFrame WRITE setStartFrame NOTIFY startFrameChanged)
    Q_PROPERTY(int endFrame READ endFrame WRITE setEndFrame NOTIFY endFrameChanged)

public:
    explicit Document(QObject *parent = nullptr);

    void toPb(pb::Document& pb) const;
    void fromPb(const pb::Document& pb);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int,QByteArray> roleNames() const override;

    Q_INVOKABLE Strip* createStrip();
    Q_INVOKABLE void deleteStrip(Strip* strip);
    Q_INVOKABLE QVariantList strips(); // QVariantList for QML
    Q_INVOKABLE void save(const QString& fileName);
    Q_INVOKABLE void load(const QString& fileName);

    int framesPerSecond() const;
    void setFramesPerSecond(int framesPerSecond);

    int startFrame() const;
    void setStartFrame(int startFrame);

    int endFrame() const;
    void setEndFrame(int endFrame);

signals:

    void framesPerSecondChanged();
    void startFrameChanged();
    void endFrameChanged();

public slots:
};

#endif // DOCUMENT_H

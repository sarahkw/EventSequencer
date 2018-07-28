#ifndef RESOURCE_H
#define RESOURCE_H

#include <QObject>
#include <QString>
#include <QUrl>

namespace pb {
class Resource;
}

class Resource : public QObject
{
    Q_OBJECT

public:
    enum class Type {
        None,
        FilePath,
        ManagedId
    };
    Q_ENUMS(Type)
private:
    Type type_ = Type::None;
    QString identifier_;

    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString identifier READ identifier WRITE setIdentifier NOTIFY identifierChanged)

public:
    explicit Resource(QObject *parent = nullptr);

    void toPb(pb::Resource& pb) const;
    void fromPb(const pb::Resource& pb);

    Type type() const;
    void setType(const Type &type);

    QString identifier() const;
    void setIdentifier(const QString &identifier);

    Q_INVOKABLE void setFilePathFromUrl(QUrl url);

signals:

    void typeChanged();
    void identifierChanged();

public slots:
};

#endif // RESOURCE_H

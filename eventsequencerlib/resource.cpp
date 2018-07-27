#include "resource.h"

#include <eventsequencer.pb.h>

Resource::Type Resource::type() const
{
    return type_;
}

void Resource::setType(const Type &type)
{
    if (type_ != type) {
        type_ = type;
        emit typeChanged();
    }
}

QString Resource::identifier() const
{
    return identifier_;
}

void Resource::setIdentifier(const QString &identifier)
{
    if (identifier_ != identifier) {
        identifier_ = identifier;
        emit identifierChanged();
    }
}

void Resource::setFilePathFromUrl(QUrl url)
{
    setType(Type::FilePath);
    setIdentifier(url.toLocalFile());
}

Resource::Resource(QObject *parent) : QObject(parent)
{
}

void Resource::toPb(pb::Resource &pb) const
{
    pb.Clear();
    switch (type()) {
    case Type::None:
        break;
    case Type::FilePath:
        *pb.mutable_filepath() = identifier_.toStdString();
        break;
    case Type::ManagedId:
        *pb.mutable_managedid() = identifier_.toStdString();
        break;
    }
}

void Resource::fromPb(const pb::Resource &pb)
{
    switch (pb.reference_case()) {
    case pb::Resource::REFERENCE_NOT_SET:
        setType(Type::None);
        break;
    case pb::Resource::kFilePath:
        setType(Type::FilePath);
        setIdentifier(QString::fromStdString(pb.filepath()));
        break;
    case pb::Resource::kManagedId:
        setType(Type::ManagedId);
        setIdentifier(QString::fromStdString(pb.managedid()));
        break;
    }
}

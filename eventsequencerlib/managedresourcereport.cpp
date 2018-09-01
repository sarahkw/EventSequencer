#include "managedresourcereport.h"

#include "managedresources.h"
#include "strip.h"

#include "document.h"

#include <QFile>

QVariantList ManagedResourceReport::stripsMissingResource() const
{
    return stripsMissingResource_;
}

bool ManagedResourceReport::hasData() const
{
    return hasData_;
}

void ManagedResourceReport::setHasData(bool hasData)
{
    if (hasData_ != hasData) {
        hasData_ = hasData;
        emit hasDataChanged();
    }
}

QVariantList ManagedResourceReport::unusedFiles() const
{
    return unusedFiles_;
}

ManagedResourceReport::ManagedResourceReport(QObject *parent) : QObject(parent)
{

}

void ManagedResourceReport::generateReport(Document *document)
{
    clearReport();

    if (document->fileResourceDirectory().isEmpty()) {
        qWarning("Cannot generate report with empty file resource dir");
        return;
    }
    ManagedResources mr(document->fileResourceDirectory());

    std::map<QUrl, Strip*> strips;
    std::map<QUrl, QString> files;

    for (Strip* s : document->strips()) {
        if (!s->resourceUrl().isEmpty()) {
            strips[s->resourceUrl()] = s;
        }
    }

    for (std::pair<QUrl, QString>& urlPair : mr.urlList()) {
        files.insert(urlPair);
    }

    for (std::pair<const QUrl, Strip*>& pair : strips) {
        if (files.find(pair.first) == files.end()) {
            QObject::connect(pair.second, &QObject::destroyed,
                             this, &ManagedResourceReport::clearReport);
            stripsMissingResource_.push_back(QVariant::fromValue(pair.second));
        }
    }

    for (std::pair<const QUrl, QString>& pair : files) {
        if (strips.find(pair.first) == strips.end()) {
            unusedFiles_.push_back(pair.second);
        }
    }

    emit stripsMissingResourceChanged();
    emit unusedFilesChanged();

    setHasData(true);
}

void ManagedResourceReport::clearReport()
{
    setHasData(false);

    for (QVariant& qv : stripsMissingResource_) {
        auto* s = qv.value<Strip*>();
        if (s != nullptr) {
            s->disconnect(this);
        } else {
            qWarning("Unexpected nullptr as Strip when clearing report");
        }
    }
    stripsMissingResource_.clear();
    emit stripsMissingResourceChanged();

    unusedFiles_.clear();
    emit unusedFilesChanged();
}

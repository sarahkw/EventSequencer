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

    for (Strip* s : document->strips()) {
        if (!s->resourceUrl().isEmpty()) {
            QString filePath;
            mr.urlConvertToFilePath(s->resourceUrl(), &filePath);
            if (!QFile::exists(filePath)) {
                QObject::connect(s, &QObject::destroyed,
                                 this, &ManagedResourceReport::clearReport);
                stripsMissingResource_.push_back(QVariant::fromValue(s));
            }
        }
    }

    emit stripsMissingResourceChanged();

    for (QUrl& url : mr.urlList()) {
        unusedFiles_.push_back(url);
    }
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

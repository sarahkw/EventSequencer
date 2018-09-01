#include "managedresourcereport.h"

#include "managedresources.h"
#include "strip.h"

#include "document.h"

#include <QFile>
#include <QLocale>

QVariantList ManagedResourceReport::stripsMissingResource() const
{
    return stripsMissingResource_;
}

bool ManagedResourceReport::hasData() const
{
    return hasData_;
}

QVariantList ManagedResourceReport::unusedFiles() const
{
    return unusedFiles_;
}

QString ManagedResourceReport::unusedFilesSize() const
{
    return unusedFilesSize_;
}

QString ManagedResourceReport::usedFilesSize() const
{
    return usedFilesSize_;
}

QVariantList ManagedResourceReport::ignoredFileNames() const
{
    return ignoredFileNames_;
}

void ManagedResourceReport::setIgnoredFileNames(const QVariantList &ignoredFileNames)
{
    if (ignoredFileNames_ != ignoredFileNames) {
        ignoredFileNames_ = ignoredFileNames;
        emit ignoredFileNamesChanged();
    }
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

    std::set<QString> ignored;
    for (QVariant& v : ignoredFileNames_) {
        auto s = v.toString();
        if (s.isEmpty()) {
            qWarning("Got an empty ignored file name");
        } else {
            ignored.insert(s);
        }
    }

    std::map<QUrl, Strip*> strips;
    std::map<QUrl, QFileInfo> files;

    qint64 unusedFilesBytes = 0;
    qint64 usedFilesBytes = 0;

    for (Strip* s : document->strips()) {
        if (!s->resourceUrl().isEmpty()) {
            strips[s->resourceUrl()] = s;
        }
    }

    for (std::pair<QUrl, QFileInfo>& urlPair : mr.urlList()) {
        files.insert(urlPair);
    }

    for (std::pair<const QUrl, Strip*>& pair : strips) {
        if (files.find(pair.first) == files.end()) {
            QObject::connect(pair.second, &QObject::destroyed,
                             this, &ManagedResourceReport::clearReport);
            stripsMissingResource_.push_back(QVariant::fromValue(pair.second));
        }
    }

    for (std::pair<const QUrl, QFileInfo>& pair : files) {
        if (strips.find(pair.first) == strips.end()) {
            auto fileName = pair.second.fileName();
            if (ignored.find(fileName) == ignored.end()) {
                unusedFiles_.push_back(fileName);
                unusedFilesBytes += pair.second.size();
            }
        } else {
            usedFilesBytes += pair.second.size();
        }
    }

    QLocale locale;
    unusedFilesSize_ = locale.formattedDataSize(unusedFilesBytes);
    usedFilesSize_ = locale.formattedDataSize(usedFilesBytes);

    hasData_ = true;
    emit reportChanged();
}

void ManagedResourceReport::clearReport()
{
    hasData_ = false;

    for (QVariant& qv : stripsMissingResource_) {
        auto* s = qv.value<Strip*>();
        if (s != nullptr) {
            s->disconnect(this);
        } else {
            qWarning("Unexpected nullptr as Strip when clearing report");
        }
    }
    stripsMissingResource_.clear();
    unusedFiles_.clear();

    unusedFilesSize_.clear();
    usedFilesSize_.clear();

    emit reportChanged();
}

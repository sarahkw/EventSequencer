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

QString ManagedResourceReport::stripsMissingResourceAsText()
{
    QStringList lines;
    for (QVariant& v : stripsMissingResource_) {
        Strip* s = v.value<Strip*>();
        if (s != nullptr) {
            lines.push_back(QString("%1, %2")
                                .arg(s->startFrame())
                                .arg(ManagedResources::urlConvertToFileName(
                                    s->resourceUrl().toString())));
        } else {
            qWarning("Null strip?");
        }
    }
    return lines.join("\n");
}

QString ManagedResourceReport::unusedFilesAsText()
{
    QStringList lines;
    for (QVariant& v : unusedFiles_) {
        lines.push_back(v.toString());
    }
    return lines.join("\n");
}

void ManagedResourceReport::deleteAllStripsMissingResources(Document* document)
{
    std::vector<Strip*> stripsToDelete;

    for (QVariant& v : stripsMissingResource_) {
        Strip* s = v.value<Strip*>();
        if (s != nullptr) {
            stripsToDelete.push_back(s);
        } else {
            qWarning("Null strip?");
        }
    }

    // When we start deleting strips, it will clear the report anyway. I think
    // it's cleaner to just do it now.
    clearReport();

    for (Strip* s : stripsToDelete) {
        document->deleteStrip(s);
    }
}

QString ManagedResourceReport::deleteAllUnusedFiles(Document* document)
{

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
        // Casting to QObject* instead of Strip* because the Strip part would
        // already be destroyed if this is a response to the destroyed signal.
        auto* s = qv.value<QObject*>();
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

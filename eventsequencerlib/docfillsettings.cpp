#include "docfillsettings.h"

#include <QStandardPaths>

static const char KEY_DOCUMENTSPATH[] = "documentsPath";

QString DocFillSettings::defaultDocumentsPath() const
{
    return defaultDocumentsPath_;
}

QString DocFillSettings::documentsPath() const
{
    if (documentsPath_.isEmpty()) {
        return defaultDocumentsPath_;
    } else {
        return documentsPath_;
    }
}

void DocFillSettings::setDocumentsPath(const QString &documentsPath)
{
    if (documentsPath_ != documentsPath) {
        settings_.setValue(KEY_DOCUMENTSPATH, documentsPath);
        documentsPath_ = documentsPath;
        emit documentsPathChanged();
    }
}

void DocFillSettings::unsetDocumentsPath()
{
    settings_.remove(KEY_DOCUMENTSPATH);
    documentsPath_.clear();
    emit documentsPathChanged();
}

bool DocFillSettings::documentsPathIsDefault() const
{
    return documentsPath_.isEmpty();
}

DocFillSettings::DocFillSettings(QObject* parent)
    : QObject(parent), settings_("DocFillInc", "DocFill") // TODO This is fake
{
    const char DEFAULT_PATH[] = "/DocFill";
#ifdef Q_OS_ANDROID
    QStandardPaths::StandardLocation location = QStandardPaths::GenericDataLocation;
#else
    QStandardPaths::StandardLocation location = QStandardPaths::HomeLocation;
#endif
    defaultDocumentsPath_ = QStandardPaths::writableLocation(location) + DEFAULT_PATH;

    documentsPath_ = settings_.value(KEY_DOCUMENTSPATH, "").toString();
}

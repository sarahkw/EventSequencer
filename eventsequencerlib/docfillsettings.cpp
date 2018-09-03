#include "docfillsettings.h"

#include <QStandardPaths>

static const char KEY_DOCUMENTSPATH[]     = "documentsPath";
static const char KEY_FONTPOINTSIZE[]     = "fontPointSize";
static const char KEY_TONEFREQUENCY[]     = "toneFrequency";
static const char KEY_REVIEWAFTERRECORD[] = "reviewAfterRecord";
static const char KEY_SELECTONRECORD[]    = "selectOnRecord";
static const char KEY_TONEBEFORERECORD[]  = "toneBeforeRecord";
static const char KEY_DEVELOPEROPTIONS[]  = "developerOptions";

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

int DocFillSettings::fontPointSize() const
{
    return fontPointSize_;
}

void DocFillSettings::setFontPointSize(int fontPointSize)
{
    if (fontPointSize_ != fontPointSize) {
        settings_.setValue(KEY_FONTPOINTSIZE, fontPointSize);
        fontPointSize_ = fontPointSize;
        emit fontPointSizeChanged();
    }
}

int DocFillSettings::toneFrequency() const
{
    return toneFrequency_;
}

void DocFillSettings::setToneFrequency(int toneFrequency)
{
    if (toneFrequency_ != toneFrequency) {
        settings_.setValue(KEY_TONEFREQUENCY, toneFrequency);
        toneFrequency_ = toneFrequency;
        emit cheapUserSettingsChanged();
    }
}

bool DocFillSettings::reviewAfterRecord() const
{
    return reviewAfterRecord_;
}

void DocFillSettings::setReviewAfterRecord(bool reviewAfterRecord)
{
    if (reviewAfterRecord_ != reviewAfterRecord) {
        settings_.setValue(KEY_REVIEWAFTERRECORD, reviewAfterRecord);
        reviewAfterRecord_ = reviewAfterRecord;
        emit cheapUserSettingsChanged();
    }
}

bool DocFillSettings::selectOnRecord() const
{
    return selectOnRecord_;
}

void DocFillSettings::setSelectOnRecord(bool selectOnRecord)
{
    if (selectOnRecord_ != selectOnRecord) {
        settings_.setValue(KEY_SELECTONRECORD, selectOnRecord);
        selectOnRecord_ = selectOnRecord;
        emit cheapUserSettingsChanged();
    }
}

bool DocFillSettings::toneBeforeRecord() const
{
    return toneBeforeRecord_;
}

void DocFillSettings::setToneBeforeRecord(bool toneBeforeRecord)
{
    if (toneBeforeRecord_ != toneBeforeRecord) {
        settings_.setValue(KEY_TONEBEFORERECORD, toneBeforeRecord);
        toneBeforeRecord_ = toneBeforeRecord;
        emit cheapUserSettingsChanged();
    }
}

bool DocFillSettings::developerOptions() const
{
    return developerOptions_;
}

void DocFillSettings::setDeveloperOptions(bool developerOptions)
{
    if (developerOptions_ != developerOptions) {
        settings_.setValue(KEY_DEVELOPEROPTIONS, developerOptions);
        developerOptions_ = developerOptions;
        emit cheapUserSettingsChanged();
    }
}

DocFillSettings::DocFillSettings(QObject* parent)
    : QObject(parent), settings_("DocFillInc", "DocFill") // TODO This is fake
{
    qWarning("TODO: Fake settings company/application name");

    const char DEFAULT_PATH[] = "/DocFill";
#ifdef Q_OS_ANDROID
    QStandardPaths::StandardLocation location = QStandardPaths::GenericDataLocation;
#else
    QStandardPaths::StandardLocation location = QStandardPaths::HomeLocation;
#endif
    defaultDocumentsPath_ = QStandardPaths::writableLocation(location) + DEFAULT_PATH;

    documentsPath_ = settings_.value(KEY_DOCUMENTSPATH, "").toString();

    fontPointSize_ = settings_.value(KEY_FONTPOINTSIZE, QVariant::fromValue(16)).toInt();

    toneFrequency_     = settings_.value(KEY_TONEFREQUENCY, QVariant::fromValue(0)).toInt();
    reviewAfterRecord_ = settings_.value(KEY_REVIEWAFTERRECORD, QVariant::fromValue(false)).toBool();
    selectOnRecord_    = settings_.value(KEY_SELECTONRECORD, QVariant::fromValue(false)).toBool();
    toneBeforeRecord_  = settings_.value(KEY_TONEBEFORERECORD, QVariant::fromValue(false)).toBool();
    developerOptions_  = settings_.value(KEY_DEVELOPEROPTIONS, QVariant::fromValue(false)).toBool();
}

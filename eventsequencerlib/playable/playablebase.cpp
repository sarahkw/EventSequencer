#include "playablebase.h"

#include <aufileheader.h>
#include <endianmodifyingiodevice.h>
#include <concatiodevice.h>

#include <managedresources.h>
#include <memory>

#include <QFile>

namespace playable {

PlayableBase::PlayableBase(QObject *parent) : QObject(parent)
{

}

void PlayableBase::setError(QString error)
{
    error_ = error;
}

QString PlayableBase::error()
{
    return error_;
}

QString PlayableBase::fileResourceDirectory() const
{
    return fileResourceDirectory_;
}

void PlayableBase::setFileResourceDirectory(const QString &fileResourceDirectory)
{
    if (fileResourceDirectory_ != fileResourceDirectory) {
        fileResourceDirectory_ = fileResourceDirectory;
        emit fileResourceDirectoryChanged();
    }
}

bool PlayableBase::appendUrlToConcatIODevice(ConcatIODevice& playingDevice,
                                             QUrl url,
                                             const QAudioFormat& outputFormat)
{
    setError("");

    ManagedResources managedResources(fileResourceDirectory());

    QString fileName;
    if (!managedResources.urlConvertToFilePath(url, &fileName)) {
        setError("Missing file resource directory");
        return false;
    }

    std::shared_ptr<QFile> muhFile(new QFile(fileName));
    if (!muhFile->open(QFile::ReadOnly)) {
        setError(QString("Cannot open: %1").arg(muhFile->errorString()));
        return false;
    }

    AuFileHeader afh;
    if (!afh.loadFileAndSeek(*muhFile)) {
        // TODO Compare the format
        setError("Cannot load AU file and seek");
        return false;
    }

    auto* emiod = new EndianModifyingIODevice(
        muhFile, unsigned(outputFormat.sampleSize() / 8),
        EndianModifyingIODevice::Big,
        outputFormat.byteOrder() == QAudioFormat::BigEndian
            ? EndianModifyingIODevice::Big
            : EndianModifyingIODevice::Little);

    const bool success = emiod->open(QIODevice::ReadOnly);
    Q_ASSERT(success); // This can't fail because inferior is already open

    playingDevice.append(emiod);
    return true;
}

} // namespace playable

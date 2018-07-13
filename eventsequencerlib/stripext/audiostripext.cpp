#include "audiostripext.h"

#include <eventsequencer.pb.h>

namespace stripext {

QString AudioStripExt::fileName() const
{
    return fileName_;
}

void AudioStripExt::setFileName(const QString &fileName)
{
    if (fileName_ != fileName) {
        fileName_ = fileName;
        emit fileNameChanged();

        fileNameUrl_.fromLocalFile(fileName);
        emit fileNameUrlChanged();
    }
}

AudioStripExt::AudioStripExt(QObject *parent) : QObject(parent)
{

}

QUrl AudioStripExt::fileNameUrl() const
{
    return fileNameUrl_;
}

void AudioStripExt::setFileNameUrl(const QUrl &fileNameUrl)
{
    if (fileNameUrl_ != fileNameUrl) {
        fileNameUrl_ = fileNameUrl;
        emit fileNameUrlChanged();

        fileName_ = fileNameUrl.toLocalFile();
        emit fileNameChanged();
    }
}

void AudioStripExt::toPb(pb::Strip_Audio &pb) const
{
    pb.set_filename(fileName_.toStdString());
}

void AudioStripExt::fromPb(const pb::Strip_Audio &pb)
{
    setFileName(QString::fromStdString(pb.filename()));
}

} // namespace stripext

#include "badaudiostripext.h"

#include <eventsequencer.pb.h>

namespace stripext {

QString BadAudioStripExt::fileName() const
{
    return fileName_;
}

void BadAudioStripExt::setFileName(const QString &fileName)
{
    if (fileName_ != fileName) {
        fileName_ = fileName;
        emit fileNameChanged();

        fileNameUrl_.fromLocalFile(fileName);
        emit fileNameUrlChanged();
    }
}

BadAudioStripExt::BadAudioStripExt(QObject *parent) : QObject(parent)
{

}

QUrl BadAudioStripExt::fileNameUrl() const
{
    return fileNameUrl_;
}

void BadAudioStripExt::setFileNameUrl(const QUrl &fileNameUrl)
{
    if (fileNameUrl_ != fileNameUrl) {
        fileNameUrl_ = fileNameUrl;
        emit fileNameUrlChanged();

        fileName_ = fileNameUrl.toLocalFile();
        emit fileNameChanged();
    }
}

void BadAudioStripExt::toPb(pb::Strip_BadAudio &pb) const
{
    pb.set_filename(fileName_.toStdString());
}

void BadAudioStripExt::fromPb(const pb::Strip_BadAudio &pb)
{
    setFileName(QString::fromStdString(pb.filename()));
}

} // namespace stripext

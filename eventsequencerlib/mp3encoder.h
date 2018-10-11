#ifndef MP3ENCODER_H
#define MP3ENCODER_H

#include "supportedaudioformat.h"

#include <QIODevice>
#include <QAudioFormat>

#include <vector>

class Mp3Encoder {
    Q_DISABLE_COPY(Mp3Encoder)

    // An already opened device that we will read raw sample data from, until
    // the end.
    QIODevice* source_;

    // Audio format of the source.
    QAudioFormat* sourceFormat_;

    // Device to write to.
    QIODevice* destination_;

    int mp3Quality_;

private:
    const size_t bufferSize_ = 16384; // QIODEVICE_BUFFERSIZE

    struct lame_global_struct* lame_ = nullptr;

    size_t frameCount_;
    size_t sampleCount_;

    std::vector<unsigned char> mp3Buffer_;

    SupportedAudioFormat::Type sampleType_;

    std::vector<float> samplesFloat_;
    std::vector<short> samplesShort_;
    std::vector<int> samplesInt_;

public:
    Mp3Encoder(QIODevice* source, QAudioFormat* sourceFormat,
               QIODevice* destination, int mp3Quality);
    ~Mp3Encoder();

    struct Result {
        bool success;
        QString errorMessage;
    };

    Result init();
private:
    Result fillBuffer(size_t* framesRead);
    void encodeBufferMono(size_t framesRead, int* encodeBufferResult);
    void encodeBufferStereo(size_t framesRead, int* encodeBufferResult);
public:
    Result encode();

    Result initAndEncode();
};

#endif // MP3ENCODER_H

#ifndef SUPPORTEDAUDIOFORMAT_H
#define SUPPORTEDAUDIOFORMAT_H

#include <QAudioFormat>

// These are the types we chose to support because they're common and easy to
// support.
class SupportedAudioFormat
{
    SupportedAudioFormat() = delete;
public:
    enum class Type {
        NotSupported,
        Float,
        Short,
        Int
    };

    static Type classify(const QAudioFormat& audioFormat);

};

#endif // SUPPORTEDAUDIOFORMAT_H

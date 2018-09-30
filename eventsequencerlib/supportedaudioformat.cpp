#include "supportedaudioformat.h"

SupportedAudioFormat::Type SupportedAudioFormat::classify(const QAudioFormat &audioFormat)
{
    if (audioFormat.sampleType() == QAudioFormat::Float) {
        if (audioFormat.sampleSize() == 32) {
            return Type::Float;
        } else {
            return Type::NotSupported;
        }
    } else if (audioFormat.sampleType() == QAudioFormat::SignedInt) {
        if (audioFormat.sampleSize() == 16) {
            return Type::Short;
        } else if (audioFormat.sampleSize() == 32) {
            return Type::Int;
        } else {
            return Type::NotSupported;
        }
    } else {
        return Type::NotSupported;
    }
}

#ifndef AUFILEHEADER_H
#define AUFILEHEADER_H

#include <QAudioFormat>
#include <QIODevice>

class AuFileHeader {
    static constexpr unsigned int UNKNOWN_DATASIZE = 0xffffffff;

    QAudioFormat audioFormat_;
    unsigned int dataSize_ = UNKNOWN_DATASIZE;

public:

    bool loadFormat(const QAudioFormat& af);
    bool loadFileAndSeek(QIODevice& device, std::string* annotation=nullptr);

    unsigned int dataSize();
    void setDataSize(unsigned int dataSize);

    bool writeFile(QIODevice& device, const std::string& annotation=std::string());
    const QAudioFormat& audioFormat();

};

#endif // AUFILEHEADER_H

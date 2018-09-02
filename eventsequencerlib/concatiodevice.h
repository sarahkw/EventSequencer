#ifndef CONCATIODEVICE_H
#define CONCATIODEVICE_H

#include <QIODevice>
#include <list>
#include <functional>

class ConcatIODevice : public QIODevice
{
    struct Child {
        QIODevice* iod;
        std::function<void()> callback;
        bool called;
    };
    std::list<Child> inputs_;

protected:

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

public:
    ConcatIODevice();
    ~ConcatIODevice() override;

    bool event(QEvent *event) override;

    // We'll take ownership of iod and delete as needed
    void append(QIODevice* iod, std::function<void()> callback=std::function<void()>());
};

#endif // CONCATIODEVICE_H

#ifndef WAITFOR_H
#define WAITFOR_H

#include <QObject>

#include <functional>

class WaitFor : public QObject
{
    Q_OBJECT

    using UnregisterFn = std::function<void(WaitFor*)>;

    UnregisterFn unregisterFn_;

    QObject* result_ = nullptr;

    Q_PROPERTY(QObject* result READ result WRITE setResult NOTIFY resultChanged)

public:
    explicit WaitFor(UnregisterFn&& unregisterFn);
    ~WaitFor();

    QObject *result() const;
    void setResult(QObject *result);

    void eraseUnregisterFn();

signals:

    void resultAboutToUnset(QObject* result);
    void resultChanged(QObject* result);

};

#endif // WAITFOR_H

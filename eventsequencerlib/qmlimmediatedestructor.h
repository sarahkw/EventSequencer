#ifndef QMLIMMEDIATEDESTRUCTOR_H
#define QMLIMMEDIATEDESTRUCTOR_H

#include <QObject>
#include <vector>

class QmlImmediateDestructor : public QObject
{
    Q_OBJECT
    std::vector<QObject*> immediateDelete_;
public:
    explicit QmlImmediateDestructor(QObject *parent = nullptr);
    ~QmlImmediateDestructor() override;

    Q_INVOKABLE void add(QObject* obj);

signals:

public slots:
};

Q_DECLARE_METATYPE(QmlImmediateDestructor*)

#endif // QMLIMMEDIATEDESTRUCTOR_H

#ifndef QMLOBJECTDESTROYEDNOTIFIER_H
#define QMLOBJECTDESTROYEDNOTIFIER_H

#include <QObject>

class QmlObjectDestroyedNotifier : public QObject
{
    Q_OBJECT

    QObject* target_ = nullptr;
    Q_PROPERTY(QObject* target READ target WRITE setTarget NOTIFY targetChanged)

public:
    explicit QmlObjectDestroyedNotifier(QObject *parent = nullptr);

    QObject *target() const;
    void setTarget(QObject *target);

signals:

    void targetChanged();

    void targetDestroyed();

public slots:
};

#endif // QMLOBJECTDESTROYEDNOTIFIER_H

#include "qmlobjectdestroyednotifier.h"

QObject *QmlObjectDestroyedNotifier::target() const
{
    return target_;
}

void QmlObjectDestroyedNotifier::setTarget(QObject *target)
{
    if (target_ != target) {
        target_ = target;
        if (target_ != nullptr) {
            QObject::connect(target_, &QObject::destroyed,
                             this, &QmlObjectDestroyedNotifier::targetDestroyed);
        }
        emit targetChanged();
    }
}

QmlObjectDestroyedNotifier::QmlObjectDestroyedNotifier(QObject *parent) : QObject(parent)
{

}

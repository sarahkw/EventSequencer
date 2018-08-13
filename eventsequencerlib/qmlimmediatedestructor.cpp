#include "qmlimmediatedestructor.h"

QmlImmediateDestructor::QmlImmediateDestructor(QObject *parent) : QObject(parent)
{

}

QmlImmediateDestructor::~QmlImmediateDestructor()
{
    for (QObject* obj : immediateDelete_) {
        delete obj;
    }
}

void QmlImmediateDestructor::add(QObject *obj)
{
    immediateDelete_.push_back(obj);
}

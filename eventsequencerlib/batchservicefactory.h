#ifndef BATCHSERVICEFACTORY_H
#define BATCHSERVICEFACTORY_H


#include <QtGlobal>
#ifdef Q_OS_ANDROID
#include "batchservicereplica.h"
#else
#include "batchserviceimpl.h"
#endif

class BatchServiceFactory
{
    BatchServiceFactory() = delete;
public:

#ifdef Q_OS_ANDROID
    using Type = BatchServiceReplica;
#else
    using Type = BatchServiceImpl;
#endif
    static Type* create() { return new Type; }

};

#endif // BATCHSERVICEFACTORY_H

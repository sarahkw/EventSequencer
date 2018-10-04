#ifndef ANDROIDLIB_SERVICEMAIN_H
#define ANDROIDLIB_SERVICEMAIN_H

#include <batchservicelib/batchserviceimplbase.h>

namespace androidlib {

class ServiceMain
{
    ServiceMain() = delete;
public:
    static int serviceMain(batchservicelib::BatchServiceImplBase* service,
                           int &argc, char **argv);
};

} // namespace androidlib

#endif // ANDROIDLIB_SERVICEMAIN_H

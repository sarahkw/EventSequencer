#ifndef ANDROIDLIB_SERVICEMAIN_H
#define ANDROIDLIB_SERVICEMAIN_H

class QObject;

namespace androidlib {

class ServiceMain
{
    ServiceMain() = delete;
public:
    static int serviceMain(QObject* service, int &argc, char **argv);
};

} // namespace androidlib

#endif // ANDROIDLIB_SERVICEMAIN_H

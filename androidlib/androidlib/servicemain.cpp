#include "servicemain.h"

#include "servicebinder.h"

#include <QAndroidService>

#include <memory>

namespace androidlib {

int ServiceMain::serviceMain(QObject* service, int &argc, char **argv)
{
    std::shared_ptr<QObject> serviceSp(service);
    QAndroidService app(argc, argv, [serviceSp](const QAndroidIntent&) -> QAndroidBinder* {
        return new ServiceBinder(serviceSp);
    });

    int result = app.exec();

    // TODO Tell the service that we're exiting, somehow.

    return result;
}

} // namespace androidlib

#include "errorreportingcontext.h"

#include "conditionalerror.h"

// Test!
//#define DEMO

QStringList ErrorReportingContext::model() const
{
    // XXX It sucks to abuse strings like that, but I have to ship.
    QStringList ret;
#ifdef DEMO
    ret.push_back("AD If you find this software enjoyable or useful, please consider purchasing it. Thanks!");
#endif
    for (ConditionalError* cerror : activeConditionalErrors_) {
        ret.push_back(QString("ERROR %1").arg(cerror->errorText()));
    }
    return ret;
}

ErrorReportingContext::ErrorReportingContext(QObject *parent) : QObject(parent)
{

}

void ErrorReportingContext::registerConditionalError(ConditionalError *cerror)
{
    QObject::connect(cerror, &QObject::destroyed,
                     this, &ErrorReportingContext::destroyedConditionalError);
    QObject::connect(cerror, &ConditionalError::errorTextChanged,
                     this, &ErrorReportingContext::modelChanged);
    activeConditionalErrors_.push_back(cerror);
    emit modelChanged();
}

void ErrorReportingContext::unregisterConditionalError(ConditionalError *cerror)
{
    cerror->disconnect(this);
    destroyedConditionalError(cerror);
}

void ErrorReportingContext::destroyedConditionalError(QObject *object)
{
    ConditionalError* cerror = qobject_cast<ConditionalError*>(object);
    if (cerror == nullptr) {
        qWarning("ErrorReportingContext destroyed wrong object type");
        return;
    }

    auto iter = std::find(activeConditionalErrors_.begin(),
                          activeConditionalErrors_.end(),
                          cerror);
    if (iter != activeConditionalErrors_.end()) {
        activeConditionalErrors_.erase(iter);
    } else {
        qWarning("ErrorReportingContext failed to find error to unregister");
    }
    emit modelChanged();
}

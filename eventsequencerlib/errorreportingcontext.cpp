#include "errorreportingcontext.h"

#include "conditionalerror.h"

QStringList ErrorReportingContext::model() const
{
    QStringList ret;
    for (ConditionalError* cerror : activeConditionalErrors_) {
        ret.push_back(cerror->errorText());
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

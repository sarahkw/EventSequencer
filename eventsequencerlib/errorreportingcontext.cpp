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
    // This was a bug that I'm now keeping here as a record that the
    // bug existed, so that it is less likely that it will happen
    // again.

//    ConditionalError* cerror = qobject_cast<ConditionalError*>(object);
//    if (cerror == nullptr) {
//        qWarning("ErrorReportingContext destroyed wrong object type");
//        return;
//    }

    bool found = false;
    for (auto iter = activeConditionalErrors_.begin();
         iter != activeConditionalErrors_.end();
         ++iter) {

        if (static_cast<QObject*>(*iter) == object) {
            found = true;
            activeConditionalErrors_.erase(iter);
            break;
        }
    }

    if (!found) {
        qWarning("ErrorReportingContext failed to find error to unregister");
    }

    emit modelChanged();
}

#include "conditionalerror.h"

#include "errorreportingcontext.h"

ErrorReportingContext *ConditionalError::errorReportingContext() const
{
    return errorReportingContext_;
}

void ConditionalError::setErrorReportingContext(ErrorReportingContext *errorReportingContext)
{
    if (errorReportingContext_ != errorReportingContext) {
        if (errorReportingContext_ != nullptr) {
            if (iAmRegistered_) {
                errorReportingContext_->unregisterConditionalError(this);
                iAmRegistered_ = false;
            }
            errorReportingContext_->disconnect(this);
        }
        errorReportingContext_ = errorReportingContext;
        if (errorReportingContext_ != nullptr) {
            QObject::connect(errorReportingContext_, &QObject::destroyed,
                             this, &ConditionalError::destroyedErrorReportingContext);
        }
        emit errorReportingContextChanged();
        checkRegistration();
    }
}

void ConditionalError::destroyedErrorReportingContext()
{
    iAmRegistered_ = false;
    errorReportingContext_ = nullptr;
}

bool ConditionalError::active() const
{
    return active_;
}

void ConditionalError::setActive(bool active)
{
    if (active_ != active) {
        active_ = active;
        emit activeChanged();
        checkRegistration();
    }
}

QString ConditionalError::errorText() const
{
    return errorText_;
}

void ConditionalError::setErrorText(const QString &errorText)
{
    if (errorText_ != errorText) {
        errorText_ = errorText;
        emit errorTextChanged();
    }
}

void ConditionalError::checkRegistration()
{
    if (errorReportingContext_ != nullptr) {
        if (active_ != iAmRegistered_) {
            if (active_) {
                errorReportingContext_->registerConditionalError(this);
                iAmRegistered_ = true;
            } else {
                errorReportingContext_->unregisterConditionalError(this);
                iAmRegistered_ = false;
            }
        }
    }
}

ConditionalError::ConditionalError(QObject *parent) : QObject(parent)
{

}

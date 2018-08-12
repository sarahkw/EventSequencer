#ifndef CONDITIONALERROR_H
#define CONDITIONALERROR_H

#include <QObject>

class ErrorReportingContext;

// TODO I'd like to write a warning if the component completed without having
//      the errorReportingContext set.

class ConditionalError : public QObject
{
    Q_OBJECT

    ErrorReportingContext* errorReportingContext_ = nullptr;
    bool active_ = false;
    QString errorText_;

    Q_PROPERTY(ErrorReportingContext* errorReportingContext READ errorReportingContext WRITE setErrorReportingContext NOTIFY errorReportingContextChanged)
    Q_PROPERTY(bool                   active                READ active                WRITE setActive                NOTIFY activeChanged)
    Q_PROPERTY(QString                errorText             READ errorText             WRITE setErrorText             NOTIFY errorTextChanged)

    bool iAmRegistered_ = false;
    void checkRegistration();

public:
    explicit ConditionalError(QObject *parent = nullptr);

    ErrorReportingContext *errorReportingContext() const;
    void setErrorReportingContext(ErrorReportingContext *errorReportingContext);
    void destroyedErrorReportingContext();

    bool active() const;
    void setActive(bool active);

    QString errorText() const;
    void setErrorText(const QString &errorText);

signals:

    void errorReportingContextChanged();
    void activeChanged();
    void errorTextChanged();

public slots:
};

#endif // CONDITIONALERROR_H

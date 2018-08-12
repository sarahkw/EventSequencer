#ifndef ERRORREPORTINGCONTEXT_H
#define ERRORREPORTINGCONTEXT_H

#include <QObject>
#include <QStringList>

#include <vector>

class ConditionalError;

class ErrorReportingContext : public QObject
{
    Q_OBJECT

    std::vector<ConditionalError*> activeConditionalErrors_;

    Q_PROPERTY(QStringList model READ model NOTIFY modelChanged)

public:
    explicit ErrorReportingContext(QObject *parent = nullptr);

    void registerConditionalError(ConditionalError* cerror);
    void unregisterConditionalError(ConditionalError* cerror);
    void destroyedConditionalError(QObject* object);

    QStringList model() const;

signals:

    void modelChanged();

public slots:
};

#endif // ERRORREPORTINGCONTEXT_H

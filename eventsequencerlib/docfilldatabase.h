#ifndef DOCFILLDATABASE_H
#define DOCFILLDATABASE_H

#include <QObject>

class DocFillDatabase : public QObject
{
    Q_OBJECT

    QString errorMessage_;
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

    bool initWasSuccessful_ = false;
    void init();

public:
    explicit DocFillDatabase(QObject *parent = nullptr);

    QString errorMessage() const;
private:
    void setErrorMessage(QString errorMessage);
public:

    Q_INVOKABLE void statsAddTodayAssignedDuration(qint64 duration);

signals:

    void errorMessageChanged();

public slots:
};

#endif // DOCFILLDATABASE_H

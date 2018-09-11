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

signals:

    void errorMessageChanged();

public slots:
};

#endif // DOCFILLDATABASE_H

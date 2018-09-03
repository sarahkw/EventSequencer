#ifndef DOCUMENTMANAGERDELETER_H
#define DOCUMENTMANAGERDELETER_H

#include <QObject>

class DocumentManagerDeleter : public QObject
{
    Q_OBJECT
public:
    explicit DocumentManagerDeleter(QObject *parent = nullptr);

signals:

public slots:
};

#endif // DOCUMENTMANAGERDELETER_H
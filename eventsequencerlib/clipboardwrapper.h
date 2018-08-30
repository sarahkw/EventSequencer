#ifndef CLIPBOARDWRAPPER_H
#define CLIPBOARDWRAPPER_H

#include <QObject>

// I guess they didn't want to make a QML QClipboard wrapper so I have to do it
// myself.
class ClipboardWrapper : public QObject
{
    Q_OBJECT
public:
    explicit ClipboardWrapper(QObject *parent = nullptr);

    Q_INVOKABLE void setText(QString text) const;

signals:

public slots:
};

#endif // CLIPBOARDWRAPPER_H

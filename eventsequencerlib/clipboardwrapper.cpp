#include "clipboardwrapper.h"

#include <QClipboard>
#include <QGuiApplication>

ClipboardWrapper::ClipboardWrapper(QObject *parent) : QObject(parent)
{

}

void ClipboardWrapper::setText(QString text) const
{
    QGuiApplication::clipboard()->setText(text);
}

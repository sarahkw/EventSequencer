#include "managedresourcereport.h"

#include "document.h"

ManagedResourceReport::ManagedResourceReport(QObject *parent) : QObject(parent)
{

}

void ManagedResourceReport::generateReport(Document *document)
{
    for (Strip* s : document->strips()) {

    }
}

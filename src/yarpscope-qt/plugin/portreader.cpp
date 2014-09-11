#include "portreader.h"

PortReader *PortReader::self = NULL;

PortReader::PortReader(QObject *parent) :
    QObject(parent)
{
    self = this;
}

PortReader * PortReader::instance()
{
    if(self == NULL){
        return  new PortReader();
    }
    return self;
}

void PortReader::acquireData(const QString &remotePortName,
                     int index,
                     const QString &localPortName,
                     const QString &carrier,
                     bool persistent)
{

}

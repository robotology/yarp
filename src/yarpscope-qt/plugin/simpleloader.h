#ifndef SIMPLELOADER_H
#define SIMPLELOADER_H

#include <QObject>
#include "yarp/os/Property.h"
#include "plotmanager.h"
#include "genericloader.h"

class SimpleLoader : public GenericLoader
{
    Q_OBJECT
public:
    explicit SimpleLoader(/* FIXME const */ yarp::os::Property *options, PlotManager *plotManager,bool *ok, QObject *parent = 0);

private:
    PlotManager *plotManager;



signals:

public slots:

};

#endif // SIMPLELOADER_H

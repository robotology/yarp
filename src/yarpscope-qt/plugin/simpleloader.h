/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef SIMPLELOADER_H
#define SIMPLELOADER_H

#include <QObject>
#include "yarp/os/ResourceFinder.h"
#include "plotmanager.h"
#include "genericloader.h"

/*! \class SimpleLoader
    \brief Reads a configuration from a xml file
*/
class SimpleLoader : public GenericLoader
{
    Q_OBJECT
public:
    explicit SimpleLoader(/* FIXME const */ yarp::os::ResourceFinder *options, PlotManager *plotManager,bool *ok, QObject *parent = 0);

private:
    PlotManager *plotManager;



signals:

public slots:

};

#endif // SIMPLELOADER_H

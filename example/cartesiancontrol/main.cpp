// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/CartesianControl.h>
#include <yarp/sig/Vector.h>

#include <stdio.h>

#include "drivers.h"

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

/** Ugo Pattacini, Jun 2009.
 *  Test cartesian control interface.
 */
int main()
{
    Network yarp;    

    if (!yarp.checkNetwork())
        return -1;

    DriverCollection dev;   // register icubmoddev
    fprintf(stdout,"\nList of available devices:\n\n%s\n",dev.status().c_str());

    Property p;
    p.put("device", "cartesiancontrollerclient");
    p.put("remote", "/armCartController/server"); // prefix for remote names
    p.put("local",  "/armCartController/client"); // prefix for local names

    // we assume that somewhere the server part
    // called armCartController/server is running

    PolyDriver device;
    if (!device.open(p))
        return -1;

    ICartesianControl *ictrl;
    device.view(ictrl);

    // set trajectory time
    double time;
    ictrl->setTrajTime(2.0);
    ictrl->getTrajTime(&time);  // just for double-check
    fprintf(stdout,"trajectory time = %.1f s\n",time);

    Vector xd(3);
    xd[0]=-0.2;
    xd[1]= 0.0;
    xd[2]= 0.1;
    fprintf(stdout,"go to %s\n",xd.toString().c_str());
    ictrl->goToPosition(xd);

    fprintf(stdout,"waiting while reaching...\n");
    bool done=false;
    while (!done)
        ictrl->checkMotionDone(&done);

    fprintf(stdout,"position reached\n");

    device.close();
    return 0; 
}



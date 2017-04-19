/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_REMOTECONTROLBOARD_STATEEXTENDEDREADER_H
#define YARP_DEV_REMOTECONTROLBOARD_STATEEXTENDEDREADER_H


#include <string.h>

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Log.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/PreciselyTimed.h>

#include "jointData.h"


// encoders should arrive at least every 0.5s to be considered valide
// getEncoders will return false otherwise.
const double TIMEOUT_EXT=0.5;

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

class StateExtendedInputPort:public yarp::os::BufferedPort<jointData>
{
    jointData last;
    Semaphore mutex;
    Stamp lastStamp;
    double deltaT;
    double deltaTMax;
    double deltaTMin;
    double prev;
    double now;

    bool valid;
    int count;
public:

    StateExtendedInputPort();

    void resetStat();
    void init(int numberOfJoints);

    using yarp::os::BufferedPort<jointData>::onRead;
    virtual void onRead(jointData &v);

    // use vocab to identify the data to be read
    // get a value for a single joint
    bool getLastSingle(int j, int field, double *data, Stamp &stamp, double &localArrivalTime);
    bool getLastSingle(int j, int field, int    *data, Stamp &stamp, double &localArrivalTime);

    // get a value for all joints
    bool getLastVector(int field, double *data, Stamp &stamp, double &localArrivalTime);
    bool getLastVector(int field, int    *data, Stamp &stamp, double &localArrivalTime);
    int  getIterations();

    // time is in ms
    void getEstFrequency(int &ite, double &av, double &min, double &max);
};

#endif // YARP_DEV_REMOTECONTROLBOARD_STATEEXTENDEDREADER_H

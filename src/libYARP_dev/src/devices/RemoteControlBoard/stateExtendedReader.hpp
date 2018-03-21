/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_REMOTECONTROLBOARD_STATEEXTENDEDREADER_H
#define YARP_DEV_REMOTECONTROLBOARD_STATEEXTENDEDREADER_H


#include <cstring>

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
    virtual void onRead(jointData &v) override;

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

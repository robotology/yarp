/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_REMOTECONTROLBOARD_STATEEXTENDEDREADER_H
#define YARP_DEV_REMOTECONTROLBOARD_STATEEXTENDEDREADER_H


#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Log.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/IPreciselyTimed.h>

#include <yarp/dev/impl/jointData.h>

#include <cstring>
#include <mutex>

// encoders should arrive at least every 0.5s to be considered valide
// getEncoders will return false otherwise.

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

class StateExtendedInputPort :
        public yarp::os::BufferedPort<yarp::dev::impl::jointData>
{
    yarp::dev::impl::jointData last;
    std::mutex mutex;
    Stamp lastStamp;
    double deltaT;
    double deltaTMax;
    double deltaTMin;
    double now;
    double prev;
    double timeout;

    bool valid;
    int count;
public:

    StateExtendedInputPort();

    void resetStat();
    void init(int numberOfJoints);

    using yarp::os::BufferedPort<yarp::dev::impl::jointData>::onRead;
    void onRead(yarp::dev::impl::jointData &v) override;

    /**
     * @brief setTimeout, set the timeout for retrieving data
     * @param timeout in seconds
     */
    void setTimeout(const double& timeout);

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

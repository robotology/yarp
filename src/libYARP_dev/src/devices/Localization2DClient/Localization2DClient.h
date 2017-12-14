/*
 * Copyright (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_LOCALIZATION2DCLIENT_H
#define YARP_DEV_LOCALIZATION2DCLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/ConstString.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/ILocalization2D.h>

namespace yarp {
    namespace dev {
        class Localization2DClient;
    }
}

/**
 *  @ingroup dev_impl_wrapper
 *
 * \section Localization2DClient
 * A device which allows a user application retrieve the current position of the robot in the world.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | local          |      -         | string  | -   |   -           | Yes          | Full port name opened by the Localization2DClient device.                             |       |
 * | remote         |      -         | string  | -   |   -           | Yes          | Full port name of the port opened on the server side, to which the Localization2DClient connects to.                           | E.g.(https://github.com/robotology/navigation/src/localizationServer)    |
 */

class yarp::dev::Localization2DClient : public DeviceDriver,
                                       public ILocalization2D
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
    yarp::os::Mutex               m_mutex;
    yarp::os::Port                m_rpc_port_localization_server;
    yarp::os::ConstString         m_local_name;
    yarp::os::ConstString         m_remote_name;

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* The following methods belong to ILocalization2D interface */
    bool   getCurrentPosition(yarp::dev::Map2DLocation &loc) override;
    bool   setInitialPose(yarp::dev::Map2DLocation& loc) override;
};

#endif // YARP_DEV_LOCALIZATION2DCLIENT_H

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_FRAMETRANSFORMSERVER_H
#define YARP_DEV_FRAMETRANSFORMSERVER_H


#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IFrameTransform.h>
#include <yarp/dev/IFrameTransformClientControl.h>
#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/math/FrameTransform.h>
#include <yarp/os/PeriodicThread.h>
#include <mutex>
#include <yarp/robotinterface/Param.h>
#include <yarp/robotinterface/XMLReader.h>

#include <mutex>

#define DEFAULT_THREAD_PERIOD 20 //ms
const int MAX_PORTS = 5;

/**
 *  @ingroup dev_impl_other
 *
 * \section FrameTransformServer_device_parameters Description of input parameters
 *
 * \brief A server to manage FrameTransforms for a robot (see \ref FrameTransform)
 *
 *   Parameters required by this device are:
 * | Parameter name   | SubParameter         | Type    | Units          | Default Value        | Required     | Description                                                                                               |
 * |:----------------:|:--------------------:|:-------:|:--------------:|:--------------------:|:-----------: |:---------------------------------------------------------------------------------------------------------:|
 * | filexml_option   | -                    | string  | -              | fts_yarp_only.xml    | no           | The name of the xml file containing the needed server configuration                                       |
 * | ft_server_prefix | -                    | string  | -              | ""                   | no           | A prefix to add to the names of all the ports opened by the NWSs instantiated by the frameTransformServer |
 *
 * Example of command line:
 * \code{.unparsed}
 * yarpdev --device frameTransformServer --filexml_option fts_yarp_only.xml
 * \endcode
 *
 * Example of configuration files using .ini format.
 * \code{.unparsed}
 * device frameTransformServer
 * filexml_option fts_yarp_only.xml
 * \endcode
 */

class FrameTransformServer :
        public yarp::dev::DeviceDriver,
        public yarp::os::PortReader,
        public yarp::os::PeriodicThread
{
protected:

    yarp::os::Port      m_rpc_InterfaceToUser;
    std::string         m_local_name;
    double              m_period;
    std::mutex          m_rpc_mutex;

    //new stuff
    yarp::robotinterface::Robot m_robot;

public:
    FrameTransformServer();
    ~FrameTransformServer();

public:
    // DeviceDriver methods
    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool read(yarp::os::ConnectionReader& connection) override;

    //PeriodicThread
    bool     threadInit() override;
    void     threadRelease() override;
    void     run() override;
};

#endif // YARP_DEV_FRAMETRANSFORMSERVER_H

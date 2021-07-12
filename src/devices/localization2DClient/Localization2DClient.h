/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_LOCALIZATION2DCLIENT_H
#define YARP_DEV_LOCALIZATION2DCLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <mutex>
#include <string>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/ILocalization2D.h>


/**
 *  @ingroup dev_impl_network_clients dev_impl_navigation
 *
 * \section Localization2DClient
 *
 * \brief `localization2DClient` A device which allows a user application retrieve the current position of the robot in the world.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | local          |      -         | string  | -   |   -           | Yes          | Full port name opened by the Localization2DClient device.                             |       |
 * | remote         |      -         | string  | -   |   -           | Yes          | Full port name of the port opened on the server side, to which the Localization2DClient connects to.                           | E.g.(https://github.com/robotology/navigation/src/localizationServer)    |
 */
class Localization2DClient :
        public yarp::dev::DeviceDriver,
        public yarp::dev::Nav2D::ILocalization2D
{
protected:
    std::mutex               m_mutex;
    yarp::os::Port                m_rpc_port_localization_server;
    std::string         m_local_name;
    std::string         m_remote_name;

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* The following methods belong to ILocalization2D interface */
    bool   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation &loc) override;
    bool   getEstimatedOdometry(yarp::dev::OdometryData& odom) override;
    bool   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc) override;
    bool   getLocalizationStatus(yarp::dev::Nav2D::LocalizationStatusEnum& status) override;
    bool   getEstimatedPoses(std::vector<yarp::dev::Nav2D::Map2DLocation>& poses) override;
    bool   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc, const yarp::sig::Matrix& cov) override;
    bool   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation& loc, yarp::sig::Matrix& cov) override;
    bool   startLocalizationService() override;
    bool   stopLocalizationService() override;
};

#endif // YARP_DEV_LOCALIZATION2DCLIENT_H

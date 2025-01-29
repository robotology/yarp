/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_LOCALIZATION2D_NWC_YARP_H
#define YARP_DEV_LOCALIZATION2D_NWC_YARP_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
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
#include "ILocalization2DMsgs.h"

#include "Localization2D_nwc_yarp_ParamsParser.h"

/**
 *  @ingroup dev_impl_nwc_yarp dev_impl_navigation
 *
 * \section Localization2D_nwc_yarp
 *
 * \brief `Localization2D_nwc_yarp` A device which allows a user application retrieve the current position of the robot in the world.
 *
 *  Parameters required by this device are shown in class: Localization2D_nwc_yarp_ParamsParser
 */
class Localization2D_nwc_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::Nav2D::ILocalization2D,
        public Localization2D_nwc_yarp_ParamsParser
{
protected:
    std::mutex                    m_mutex;
    yarp::os::Port                m_rpc_port_localization_server;
    ILocalization2DMsgs           m_RPC;

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* The following methods belong to ILocalization2D interface */
    yarp::dev::ReturnValue   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation &loc) override;
    yarp::dev::ReturnValue   getEstimatedOdometry(yarp::dev::OdometryData& odom) override;
    yarp::dev::ReturnValue   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc) override;
    yarp::dev::ReturnValue   getLocalizationStatus(yarp::dev::Nav2D::LocalizationStatusEnum& status) override;
    yarp::dev::ReturnValue   getEstimatedPoses(std::vector<yarp::dev::Nav2D::Map2DLocation>& poses) override;
    yarp::dev::ReturnValue   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc, const yarp::sig::Matrix& cov) override;
    yarp::dev::ReturnValue   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation& loc, yarp::sig::Matrix& cov) override;
    yarp::dev::ReturnValue   startLocalizationService() override;
    yarp::dev::ReturnValue   stopLocalizationService() override;
};

#endif // YARP_DEV_LOCALIZATION2D_NWC_YARP_H

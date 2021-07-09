/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_LOCALIZATION2D_NWS_YARP_H
#define YARP_DEV_LOCALIZATION2D_NWS_YARP_H


#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ILocalization2D.h>
#include <yarp/dev/OdometryData.h>
#include <math.h>

 /**
 * @ingroup dev_impl_nws_yarp dev_impl_navigation
 *
 * \section Localization2D_nws_yarp
 *
 * \brief `localization2D_nws_yarp`: A localization server which can be wrap multiple algorithms and devices to provide robot localization in a 2D World.
 *
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units              | Default Value            | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:------------------:|:------------------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | GENERAL        |  period        | double  | s                  | 0.01                     | No           | The period of the working thread                                  |       |
 * | GENERAL        |  retrieve_position_periodically  | bool  | -  | true                     | No           | If true, the subdevice is asked periodically to retrieve the current location. Otherwise the current location is obtained asynchronously when a getCurrentPosition() command is issued.     | -     |
 * | GENERAL        |  name          | string  |  -                 | /localization2D_nws_yarp | No           | The name of the server, used as a prefix for the opened ports     | By default ports opened are /xxx/rpc and /xxx/streaming:o     |
 * | GENERAL        |  publish_odometry | bool |  -                 | true                     | No           | Periodically publish odometry data over the network               | -     |
 * | GENERAL        |  publish_location | bool |  -                 | true                     | No           | PEriodically publish location data over the network               | -     |
 * | subdevice      |  -             | string  |  -                 |  -                       | Yes          | The name of the of Localization device to be used                 | -     |
 */
class Localization2D_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader
{
protected:

    //yarp
    std::string                               m_local_name = "/localization2D_nws_yarp";
    yarp::os::Port                            m_rpcPort;
    std::string                               m_rpcPortName;
    yarp::os::BufferedPort<yarp::dev::Nav2D::Map2DLocation>  m_2DLocationPort;
    std::string                               m_2DLocationPortName;
    yarp::os::BufferedPort<yarp::dev::OdometryData>  m_odometryPort;
    std::string                               m_odometryPortName;
    bool                                      m_enable_publish_odometry=true;
    bool                                      m_enable_publish_location=true;

    //drivers and interfaces
    yarp::dev::PolyDriver                   pLoc;
    yarp::dev::Nav2D::ILocalization2D*      iLoc = nullptr;

    double                                  m_stats_time_last;
    double                                  m_period;
    yarp::os::Stamp                         m_loc_stamp;
    yarp::os::Stamp                         m_odom_stamp;
    bool                                    m_getdata_using_periodic_thread = true;

    yarp::dev::OdometryData                     m_current_odometry;
    yarp::dev::Nav2D::Map2DLocation             m_current_position;
    yarp::dev::Nav2D::LocalizationStatusEnum    m_current_status = yarp::dev::Nav2D::LocalizationStatusEnum::localization_status_not_yet_localized;

private:
    void publish_2DLocation_on_yarp_port();
    void publish_odometry_on_yarp_port();

public:
    Localization2D_nws_yarp();

    bool open(yarp::os::Searchable& prop) override;
    bool close() override;
    bool detach() override;
    bool attach(yarp::dev::PolyDriver* driver) override;
    void run() override;

    bool initialize_YARP(yarp::os::Searchable &config);
    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_DEV_LOCALIZATION2D_NWS_YARP_H

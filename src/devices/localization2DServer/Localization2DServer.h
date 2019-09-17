/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef YARP_DEV_LOCALIZATION2DSERVER_H
#define YARP_DEV_LOCALIZATION2DSERVER_H


#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ILocalization2D.h>
#include <math.h>

 /**
 * \section Localization2DServer
 * A localization server which can be wrap multiple algorithms and devices to provide robot in a 2D World.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value       | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | GENERAL        |  period        | double  | s              | 0.01                | No           | The period of the working thread                                  |       |
 * | GENERAL        |  retrieve_position_periodically     | bool  | -  | true         | No           | If true, the subdevice is asked periodically to retrieve the current location. Otherwise the current location is obtained asynchronously when a getCurrentPosition() command is issued.     | -     |
 * | GENERAL        |  name          | string  |  -             | /localizationServer | No           | The name of the server, used as a prefix for the opened ports     | By default ports opened are /localizationServer/rpc and /localizationServer/streaming:o     |
 * | subdevice      |  -             | string  |  -             |  -                  | Yes          | The name of the of Localization device to be used                 | -     |
 */


class Localization2DServer :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public yarp::dev::IMultipleWrapper,
        public yarp::os::PortReader
{
protected:
    yarp::os::Port                            m_rpcPort;
    std::string                               m_rpcPortName;
    yarp::os::BufferedPort<yarp::os::Bottle>  m_streamingPort;
    std::string                               m_streamingPortName;

    //drivers and interfaces
    yarp::dev::PolyDriver                   pLoc;
    yarp::dev::ILocalization2D*             iLoc;

    double                                  m_stats_time_last;
    double                                  m_period;
    bool                                    m_getdata_using_periodic_thread;
    yarp::dev::Nav2D::Map2DLocation         m_current_position;
    yarp::dev::LocalizationStatusEnum       m_current_status;

public:
    Localization2DServer();

public:
    virtual bool open(yarp::os::Searchable& prop) override;
    virtual bool close() override;
    virtual bool detachAll() override;
    virtual bool attachAll(const yarp::dev::PolyDriverList &l) override;
    virtual void run() override;

    bool initialize_YARP(yarp::os::Searchable &config);
    virtual bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_DEV_LOCALIZATION2DSERVER_H

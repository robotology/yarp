/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef LASER_FROM_EXTERNAL_PORT_H
#define LASER_FROM_EXTERNAL_PORT_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/sig/LaserScan2D.h>
#include <yarp/dev/Lidar2DDeviceBase.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/IFrameTransform.h>
#include <yarp/dev/PolyDriver.h>

#include <mutex>
#include <string>
#include <vector>

typedef unsigned char byte;

//---------------------------------------------------------------------------------------------------------------
enum base_enum
{
    BASE_IS_NAN = 0,
    BASE_IS_INF = 1,
    BASE_IS_ZERO = 2
};

class InputPortProcessor :
        public yarp::os::BufferedPort<yarp::sig::LaserScan2D>
{
    std::mutex             m_port_mutex;
    yarp::sig::LaserScan2D m_lastScan;
    yarp::os::Stamp        m_lastStamp;
    bool                   m_contains_data;

public:
    InputPortProcessor(const InputPortProcessor& alt) :
            yarp::os::BufferedPort<yarp::sig::LaserScan2D>(),
            m_lastScan(alt.m_lastScan),
            m_lastStamp(alt.m_lastStamp),
            m_contains_data(alt.m_contains_data)
    {
    }

    InputPortProcessor();
    using yarp::os::BufferedPort<yarp::sig::LaserScan2D>::onRead;
    void onRead(yarp::sig::LaserScan2D& v) override;
    void getLast(yarp::sig::LaserScan2D& data, yarp::os::Stamp& stmp);
};

/**
 * @ingroup dev_impl_lidar
 *
 * \brief `laserFromExternalPort`: Documentation to be added
 *
 * Configuration examples:
 * yarpdev --device rangefinder2D_nws_yarp --subdevice laserFromExternalPort \
 * --SENSOR::input_ports_name "(/port1 /port2)" \
 * --TRANSFORM_CLIENT::local /LaserFromExternalPort/tfClient \
 * --TRANSFORM_CLIENT::remote /transformServer \
 * --TRANSFORMS::src_frames "(/frame1 /frame2)" \
 * --TRANSFORMS::dst_frame /output_frame \
 * --period 0.01 \
 * --name /outlaser:o

 * yarpdev --device rangefinder2D_nws_yarp --subdevice laserFromExternalPort \
 * --SENSOR::min_angle 0 \
 * --SENSOR::max_angle 360 \
 * --SENSOR::resolution 0.5 \
 * --SENSOR::input_ports_name "(/port1 /port2)" \
 * --TRANSFORM_CLIENT::local /LaserFromExternalPort/tfClient \
 * --TRANSFORM_CLIENT::remote /transformServer \
 * --TRANSFORMS::src_frames "(/frame1 /frame2)" \
 * --TRANSFORMS::dst_frame /output_frame \
 * --period 0.01 \
 * --name /outlaser:o
*/

class LaserFromExternalPort : public yarp::dev::Lidar2DDeviceBase,
                              public yarp::os::PeriodicThread,
                              public yarp::dev::DeviceDriver
{
protected:
    bool                            m_option_override_limits;
    std::vector <std::string>       m_port_names;
    std::vector<InputPortProcessor> m_input_ports;
    std::vector <yarp::os::Stamp>        m_last_stamp;
    std::vector<yarp::sig::LaserScan2D> m_last_scan_data;
    yarp::dev::PolyDriver                m_tc_driver;
    yarp::dev::IFrameTransform*          m_iTc = nullptr;

    std::vector <std::string>            m_src_frame_id;
    std::string                          m_dst_frame_id;
    yarp::sig::Vector                    m_empty_laser_data;
    base_enum                            m_base_type;

    void calculate(yarp::sig::LaserScan2D scan, yarp::sig::Matrix m);

public:
    LaserFromExternalPort(double period = 0.01) : Lidar2DDeviceBase(), PeriodicThread(period)
    {
        m_option_override_limits=false;
        m_base_type = base_enum::BASE_IS_NAN;
    }

    ~LaserFromExternalPort()
    {
    }

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

public:
    //IRangefinder2D interface
    bool setDistanceRange        (double min, double max) override;
    bool setScanLimits           (double min, double max) override;
    bool setHorizontalResolution (double step) override;
    bool setScanRate             (double rate) override;

public:
    //Lidar2DDeviceBase
    bool acquireDataFromHW() override final;
};

#endif

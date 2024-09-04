/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define _USE_MATH_DEFINES

#include "Rangefinder2DTransformer.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/IFrameTransform.h>
#include <yarp/math/Math.h>
#include <yarp/math/FrameTransform.h>

#include <limits>
#include <cmath>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

namespace {
YARP_LOG_COMPONENT(RANGEFINDER2DTRANSFORMER, "yarp.device.Rangefinder2DTransformer")
}

bool Rangefinder2DTransformer::open(yarp::os::Searchable& config)
{
    if (!parseParams(config))
    {
        return false;
    }

    //get the position of the device, if it is available
    auto* drv = new yarp::dev::PolyDriver;
    Property   TransformClientOptions;
    TransformClientOptions.put("device", "transformClient");
    TransformClientOptions.put("local", "/rangefinder2DTransformClient");
    TransformClientOptions.put("remote", "/transformServer");
    TransformClientOptions.put("period", "10");

    bool b_canOpenTransformClient = false;
    if (config.check("laser_frame_name") &&
        config.check("robot_frame_name"))
    {
        m_laser_frame_name = config.find("laser_frame_name").toString();
        m_robot_frame_name = config.find("robot_frame_name").toString();
        b_canOpenTransformClient = drv->open(TransformClientOptions);
    }

    if (b_canOpenTransformClient)
    {
        yarp::dev::IFrameTransform* iTrf = nullptr;
        drv->view(iTrf);
        if (!iTrf)
        {
            yCError(RANGEFINDER2DTRANSFORMER) << "A Problem occurred while trying to view the IFrameTransform interface";
            drv->close();
            delete drv;
            return false;
        }

        yarp::sig::Matrix mat;
        iTrf->getTransform(m_laser_frame_name, m_robot_frame_name, mat);
        yarp::sig::Vector v = yarp::math::dcm2rpy(mat);
        m_device_position_x = mat[0][3];
        m_device_position_y = mat[1][3];
        m_device_position_theta = v[2];
        if (fabs(v[0]) < 1e-6 && fabs(v[1]) < 1e-6)
        {
            yCError(RANGEFINDER2DTRANSFORMER) << "Laser device is not planar";
        }
        yCInfo(RANGEFINDER2DTRANSFORMER) << "Position information obtained fromtransform server";
        drv->close();
    }
    else
    {
        if (config.check("device_position_x") &&
            config.check("device_position_y") &&
            config.check("device_position_theta"))
        {
            yCInfo(RANGEFINDER2DTRANSFORMER) << "Position information obtained from configuration parameters";
            m_device_position_x = config.find("device_position_x").asFloat64();
            m_device_position_y = config.find("device_position_y").asFloat64();
            m_device_position_theta = config.find("device_position_theta").asFloat64();
        }
        else
        {
            yCDebug(RANGEFINDER2DTRANSFORMER) << "No position information provided for this device";
        }
    }

    delete drv;
    return true;
}

bool Rangefinder2DTransformer::close()
{
    return true;
}

bool Rangefinder2DTransformer::getRawData(yarp::sig::Vector& data, double* timestamp)
{
    std::vector<LaserMeasurementData> scans;
    double lastTs;

    bool ret = sens_p->getLaserMeasurement(scans, &lastTs);
    if (!ret) {
        return false;
    }

    if (timestamp != nullptr)
    {
        *timestamp = lastTs;
    }
    return true;
}

bool Rangefinder2DTransformer::getLaserMeasurement(std::vector<LaserMeasurementData>& data, double* timestamp)
{
    std::vector<LaserMeasurementData> scans;
    double lastTs;

    bool ret = sens_p->getLaserMeasurement(scans,&lastTs);
    if (!ret)
    {
        return false;
    }
    if (isnan(m_scan_angle_min) ||
        isnan(m_scan_angle_max))
    {
        if (!sens_p->getScanLimits(m_scan_angle_min, m_scan_angle_max))
        {
            yCError(RANGEFINDER2DTRANSFORMER) << "getScanLimits failed";
            return false;
        }
    }

    size_t size = scans.size();
    data.resize(size);
    if (m_scan_angle_max < m_scan_angle_min)
    {
        yCError(RANGEFINDER2DTRANSFORMER) << "getLaserMeasurement failed";
        return false;
    }
    double laser_angle_of_view = m_scan_angle_max - m_scan_angle_min;
    for (size_t i = 0; i < size; i++)
    {
        double angle = (i / double(size) * laser_angle_of_view + m_device_position_theta + m_scan_angle_min) * DEG2RAD;
        double rho;
        double theta;
        scans[i].get_polar(rho, theta);
#if 1 //cartesian version is preferable, even if more computationally expensive, since it takes in account device_position
        data[i].set_cartesian(rho * cos(angle) + m_device_position_x, rho * sin(angle) + m_device_position_y);
#else
        data[i].set_polar(rho, theta);
#endif
    }
    if (timestamp!=nullptr)
    {
        *timestamp = lastTs;
    }
    return true;
}

bool Rangefinder2DTransformer::getDistanceRange(double& min, double& max)
{
    return sens_p->getDistanceRange(min, max);
}

bool Rangefinder2DTransformer::setDistanceRange(double min, double max)
{
    return sens_p->setDistanceRange(min, max);
}

bool Rangefinder2DTransformer::getScanLimits(double& min, double& max)
{
    return sens_p->getScanLimits(min, max);
}

bool Rangefinder2DTransformer::setScanLimits(double min, double max)
{
    return sens_p->setScanLimits(min, max);
}

bool Rangefinder2DTransformer::getHorizontalResolution(double& step)
{
    return sens_p->getHorizontalResolution(step);
}

bool Rangefinder2DTransformer::setHorizontalResolution(double step)
{
    return sens_p->setHorizontalResolution(step);
}

bool Rangefinder2DTransformer::getScanRate(double& rate)
{
    return sens_p->getScanRate(rate);
}

bool Rangefinder2DTransformer::setScanRate(double rate)
{
    return sens_p->setScanRate(rate);
}

bool Rangefinder2DTransformer::getDeviceStatus(Device_status& status)
{
    return sens_p->getDeviceStatus(status);
}

bool Rangefinder2DTransformer::getDeviceInfo(std::string& device_info)
{
    return sens_p->getDeviceInfo(device_info);
}

bool Rangefinder2DTransformer::attach(yarp::dev::PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(sens_p);
    }

    if (nullptr == sens_p)
    {
        yCError(RANGEFINDER2DTRANSFORMER, "subdevice passed to attach method is invalid");
        return false;
    }
    return true;
}

bool Rangefinder2DTransformer::detach()
{
    sens_p = nullptr;
    return true;
}
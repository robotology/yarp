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

    if (!m_laser_frame_name.empty() && !m_robot_frame_name.empty())
    {
        // get the position of the device, if it is available
        auto* drv = new yarp::dev::PolyDriver;
        Property TransformClientOptions;
        TransformClientOptions.put("device", "frameTransformClient");
        TransformClientOptions.put("local", "/rangefinder2DTransformClient");
        TransformClientOptions.put("remote", "/transformServer");
        TransformClientOptions.put("period", 0.010);
        bool b_canOpenTransformClient = drv->open(TransformClientOptions);
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
                return false;
            }
            yCInfo(RANGEFINDER2DTRANSFORMER) << "Position information obtained from frametransform server (x,y,t):" << m_device_position_x << " " << m_device_position_y << " " << m_device_position_theta;
            drv->close();
            delete drv;
        }
    }
    else
    {
        yCInfo(RANGEFINDER2DTRANSFORMER) << "Position information obtained from configuration parameters (x,y,t):" << m_device_position_x << " " << m_device_position_y << " " << m_device_position_theta;
    }

    return true;
}

bool Rangefinder2DTransformer::close()
{
    return true;
}

ReturnValue Rangefinder2DTransformer::getRawData(yarp::sig::Vector& data, double* timestamp)
{
    std::vector<LaserMeasurementData> scans;
    double lastTs;

    auto ret = sens_p->getLaserMeasurement(scans, &lastTs);
    if (!ret) {
        return ret;
    }

    if (timestamp != nullptr)
    {
        *timestamp = lastTs;
    }
    return ret;
}

ReturnValue Rangefinder2DTransformer::getLaserMeasurement(std::vector<LaserMeasurementData>& data, double* timestamp)
{
    std::vector<LaserMeasurementData> scans;
    double lastTs;

    auto ret = sens_p->getLaserMeasurement(scans,&lastTs);
    if (!ret)
    {
        return ret;
    }

    size_t size = scans.size();
    data.resize(size);
    if (m_scan_angle_max < m_scan_angle_min)
    {
        yCError(RANGEFINDER2DTRANSFORMER) << "getLaserMeasurement failed";
        return ReturnValue::return_code::return_value_error_method_failed;
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
    return ReturnValue_ok;
}

ReturnValue Rangefinder2DTransformer::getDistanceRange(double& min, double& max)
{
    return sens_p->getDistanceRange(min, max);
}

ReturnValue Rangefinder2DTransformer::setDistanceRange(double min, double max)
{
    return sens_p->setDistanceRange(min, max);
}

ReturnValue Rangefinder2DTransformer::getScanLimits(double& min, double& max)
{
    return sens_p->getScanLimits(min, max);
}

ReturnValue Rangefinder2DTransformer::setScanLimits(double min, double max)
{
    return sens_p->setScanLimits(min, max);
}

ReturnValue Rangefinder2DTransformer::getHorizontalResolution(double& step)
{
    return sens_p->getHorizontalResolution(step);
}

ReturnValue Rangefinder2DTransformer::setHorizontalResolution(double step)
{
    return sens_p->setHorizontalResolution(step);
}

ReturnValue Rangefinder2DTransformer::getScanRate(double& rate)
{
    return sens_p->getScanRate(rate);
}

ReturnValue Rangefinder2DTransformer::setScanRate(double rate)
{
    return sens_p->setScanRate(rate);
}

ReturnValue Rangefinder2DTransformer::getDeviceStatus(Device_status& status)
{
    return sens_p->getDeviceStatus(status);
}

ReturnValue Rangefinder2DTransformer::getDeviceInfo(std::string& device_info)
{
    return sens_p->getDeviceInfo(device_info);
}

bool Rangefinder2DTransformer::attach(yarp::dev::PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(sens_p);
        if (!sens_p->getScanLimits(m_scan_angle_min, m_scan_angle_max))
        {
            yCError(RANGEFINDER2DTRANSFORMER) << "getScanLimits failed";
            return false;
        }
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

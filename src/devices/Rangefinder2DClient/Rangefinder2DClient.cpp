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

#define _USE_MATH_DEFINES

#include "Rangefinder2DClient.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/IFrameTransform.h>
#include <yarp/math/Math.h>
#include <yarp/math/FrameTransform.h>

#include <limits>
#include <cmath>

/*! \file Rangefinder2DClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

inline void  Rangefinder2DInputPortProcessor::resetStat()
{
    mutex.lock();
    count=0;
    deltaT=0;
    deltaTMax=0;
    deltaTMin=1e22;
    now=SystemClock::nowSystem();
    prev=now;
    mutex.unlock();
}

Rangefinder2DInputPortProcessor::Rangefinder2DInputPortProcessor()
{
    state = IRangefinder2D::DEVICE_GENERAL_ERROR;
    resetStat();
}

void Rangefinder2DInputPortProcessor::onRead(yarp::os::Bottle &b)
{
    now=SystemClock::nowSystem();
    mutex.lock();

    if (count>0)
    {
        double tmpDT=now-prev;
        deltaT+=tmpDT;
        if (tmpDT>deltaTMax)
            deltaTMax=tmpDT;
        if (tmpDT<deltaTMin)
            deltaTMin=tmpDT;

        //compare network time
        if (tmpDT*1000<LASER_TIMEOUT)
        {
            state = b.get(1).asInt32();
        }
        else
        {
            state = IRangefinder2D::DEVICE_TIMEOUT;
        }
    }

    prev=now;
    count++;

    lastBottle=b;
    Stamp newStamp;
    getEnvelope(newStamp);

    //initialialization (first received data)
    if (lastStamp.isValid()==false)
    {
        lastStamp = newStamp;
    }

    //now compare timestamps
    if ((1000*(newStamp.getTime()-lastStamp.getTime()))<LASER_TIMEOUT)
    {
        state = b.get(1).asInt32();
    }
    else
    {
        state = IRangefinder2D::DEVICE_TIMEOUT;
    }
    lastStamp = newStamp;

    mutex.unlock();
}

inline int Rangefinder2DInputPortProcessor::getLast(yarp::os::Bottle &data, Stamp &stmp)
{
    mutex.lock();
    int ret=state;
    if (ret != IRangefinder2D::DEVICE_GENERAL_ERROR)
    {
        data=lastBottle;
        stmp = lastStamp;
    }
    mutex.unlock();

    return ret;
}

bool Rangefinder2DInputPortProcessor::getData(yarp::sig::Vector &ranges)
{
    mutex.lock();
    if (lastBottle.size()==0) { mutex.unlock(); return false; }
    unsigned int size = lastBottle.get(0).asList()->size();
    ranges.resize(size);
    for (unsigned int i = 0; i < size; i++)
        ranges[i] = lastBottle.get(0).asList()->get(i).asFloat64();
    mutex.unlock();
    return true;
}

yarp::dev::IRangefinder2D::Device_status Rangefinder2DInputPortProcessor::getStatus()
{
    mutex.lock();
    auto status = (yarp::dev::IRangefinder2D::Device_status) lastBottle.get(3).asInt32();
    mutex.unlock();
    return status;
}

inline int Rangefinder2DInputPortProcessor::getIterations()
{
    mutex.lock();
    int ret=count;
    mutex.unlock();
    return ret;
}

// time is in ms
void Rangefinder2DInputPortProcessor::getEstFrequency(int &ite, double &av, double &min, double &max)
{
    mutex.lock();
    ite=count;
    min=deltaTMin*1000;
    max=deltaTMax*1000;
    if (count<1)
    {
        av=0;
    }
    else
    {
        av=deltaT/count;
    }
    av=av*1000;
    mutex.unlock();
}

bool Rangefinder2DClient::open(yarp::os::Searchable &config)
{
    local.clear();
    remote.clear();

    local  = config.find("local").asString();
    remote = config.find("remote").asString();

    if (local=="")
    {
        yError("Rangefinder2DClient::open() error you have to provide valid local name");
        return false;
    }
    if (remote=="")
    {
        yError("Rangefinder2DClient::open() error you have to provide valid remote name");
        return false;
    }

    std::string local_rpc = local;
    local_rpc += "/rpc:o";
    std::string remote_rpc = remote;
    remote_rpc += "/rpc:i";

    if (!inputPort.open(local))
    {
        yError("Rangefinder2DClient::open() error could not open port %s, check network\n",local.c_str());
        return false;
    }
    inputPort.useCallback();

    if (!rpcPort.open(local_rpc))
    {
        yError("Rangefinder2DClient::open() error could not open rpc port %s, check network\n", local_rpc.c_str());
        return false;
    }

    bool ok=Network::connect(remote.c_str(), local.c_str(), "udp");
    if (!ok)
    {
        yError("Rangefinder2DClient::open() error could not connect to %s\n", remote.c_str());
        return false;
    }

    ok=Network::connect(local_rpc, remote_rpc);
    if (!ok)
    {
        yError("Rangefinder2DClient::open() error could not connect to %s\n", remote_rpc.c_str());
       return false;
    }

    //getScanLimits is used here to update the cached values of scan_angle_min, scan_angle_max
    double tmp_min;
    double tmp_max;
    this->getScanLimits(tmp_min, tmp_max);

    //get the position of the device, if it is available
    device_position_x = 0;
    device_position_y = 0;
    device_position_theta = 0;
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
        laser_frame_name = config.find("laser_frame_name").toString();
        robot_frame_name = config.find("robot_frame_name").toString();
        b_canOpenTransformClient = drv->open(TransformClientOptions);
    }

    if (b_canOpenTransformClient)
    {
        yarp::dev::IFrameTransform* iTrf = nullptr;
        drv->view(iTrf);
        if (!iTrf)
        {
            yError() << "A Problem occurred while trying to view the IFrameTransform interface";
            drv->close();
            delete drv;
            return false;
        }

        yarp::sig::Matrix mat;
        iTrf->getTransform(laser_frame_name, robot_frame_name, mat);
        yarp::sig::Vector v = yarp::math::dcm2rpy(mat);
        device_position_x = mat[0][3];
        device_position_y = mat[1][3];
        device_position_theta = v[2];
        if (fabs(v[0]) < 1e-6 && fabs(v[1]) < 1e-6)
        {
            yError() << "Laser device is not planar";
        }
        yInfo() << "Position information obtained fromtransform server";
        drv->close();
    }
    else
    {
        if (config.check("device_position_x") &&
            config.check("device_position_y") &&
            config.check("device_position_theta"))
        {
            yInfo() << "Position information obtained from configuration parameters";
            device_position_x = config.find("device_position_x").asFloat64();
            device_position_y = config.find("device_position_y").asFloat64();
            device_position_theta = config.find("device_position_theta").asFloat64();
        }
        else
        {
            yDebug() << "No position information provided for this device";
        }
    }

    delete drv;
    return true;
}

bool Rangefinder2DClient::close()
{
    rpcPort.close();
    inputPort.close();
    return true;
}

bool Rangefinder2DClient::getRawData(yarp::sig::Vector &data)
{
    inputPort.getData(data);
    return true;
}

bool Rangefinder2DClient::getLaserMeasurement(std::vector<LaserMeasurementData> &data)
{
    yarp::sig::Vector ranges;
    inputPort.getData(ranges);
    size_t size = ranges.size();
    data.resize(size);
    if (scan_angle_max < scan_angle_min) { yError() << "getLaserMeasurement failed"; return false; }
    double laser_angle_of_view = scan_angle_max - scan_angle_min;
    for (size_t i = 0; i < size; i++)
    {
        double angle = (i / double(size)*laser_angle_of_view + device_position_theta + scan_angle_min)* DEG2RAD;
        double value = ranges[i];
#if 1 //cartesian version is preferable, even if more computationally expensive, since it takes in account device_position
        data[i].set_cartesian(value * cos(angle) + device_position_x, value * sin(angle) + device_position_y);
#else
        data[i].set_polar(value,angle);
#endif
    }
    return true;
}

bool Rangefinder2DClient::getDistanceRange(double& min, double& max)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_DISTANCE_RANGE);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        min = response.get(2).asFloat64();
        max = response.get(3).asFloat64();
        return true;
    }
    return false;
}

bool Rangefinder2DClient::setDistanceRange(double min, double max)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_DISTANCE_RANGE);
    cmd.addFloat64(min);
    cmd.addFloat64(max);
    bool ok = rpcPort.write(cmd, response);
    if (ok)
    {
        scan_angle_min = min;
        scan_angle_max = max;
    }
    return (CHECK_FAIL(ok, response));
}

bool Rangefinder2DClient::getScanLimits(double& min, double& max)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_ANGULAR_RANGE);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        min = scan_angle_min = response.get(2).asFloat64();
        max = scan_angle_max = response.get(3).asFloat64();
        return true;
    }
    return false;
}

bool Rangefinder2DClient::setScanLimits(double min, double max)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_ANGULAR_RANGE);
    cmd.addFloat64(min);
    cmd.addFloat64(max);
    bool ok = rpcPort.write(cmd, response);
    return (CHECK_FAIL(ok, response));
}

bool Rangefinder2DClient::getHorizontalResolution(double& step)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_ANGULAR_STEP);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        step = response.get(2).asFloat64();
        return true;
    }
    return false;
}

bool Rangefinder2DClient::setHorizontalResolution(double step)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_ANGULAR_STEP);
    cmd.addFloat64(step);
    bool ok = rpcPort.write(cmd, response);
    return (CHECK_FAIL(ok, response));
}

bool Rangefinder2DClient::getScanRate(double& rate)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_SCAN_RATE);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        rate = response.get(2).asFloat64();
        return true;
    }
    return false;
}

bool Rangefinder2DClient::setScanRate(double rate)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_SCAN_RATE);
    cmd.addFloat64(rate);
    bool ok = rpcPort.write(cmd, response);
    return (CHECK_FAIL(ok, response));
}

bool Rangefinder2DClient::getDeviceStatus(Device_status &status)
{
    status = inputPort.getStatus();
    return true;
}

bool Rangefinder2DClient::getDeviceInfo(std::string &device_info)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_DEVICE_INFO);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response)!=false)
    {
        device_info = response.get(2).asString();
        return true;
    }
    return false;
}

Stamp Rangefinder2DClient::getLastInputStamp()
{
    return lastTs;
}

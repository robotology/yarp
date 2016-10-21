/*
* Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include "Rangefinder2DClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/IFrameTransform.h>
#include <yarp/math/Math.h>
#include <yarp/math/FrameTransform.h>

#include <limits>
#define _USE_MATH_DEFINES
#include <math.h>

/*! \file Rangefinder2DClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

inline void  Rangefinder2DInputPortProcessor::resetStat()
{
    mutex.wait();
    count=0;
    deltaT=0;
    deltaTMax=0;
    deltaTMin=1e22;
    now=Time::now();
    prev=now;
    mutex.post();
}

Rangefinder2DInputPortProcessor::Rangefinder2DInputPortProcessor()
{
    state = IRangefinder2D::DEVICE_GENERAL_ERROR;
    resetStat();
}

void Rangefinder2DInputPortProcessor::onRead(yarp::os::Bottle &b)
{
    now=Time::now();
    mutex.wait();

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
            state = b.get(1).asInt();
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
        state = b.get(1).asInt();
    }
    else
    {
        state = IRangefinder2D::DEVICE_TIMEOUT;
    }
    lastStamp = newStamp;

    mutex.post();
}

inline int Rangefinder2DInputPortProcessor::getLast(yarp::os::Bottle &data, Stamp &stmp)
{
    mutex.wait();
    int ret=state;
    if (ret != IRangefinder2D::DEVICE_GENERAL_ERROR)
    {
        data=lastBottle;
        stmp = lastStamp;
    }
    mutex.post();

    return ret;
}

bool Rangefinder2DInputPortProcessor::getData(yarp::sig::Vector &ranges)
{
    mutex.wait();
    if (lastBottle.size()==0) { mutex.post(); return false; }
    unsigned int size = lastBottle.get(0).asList()->size();
    ranges.resize(size);
    for (unsigned int i = 0; i < size; i++)
        ranges[i] = lastBottle.get(0).asList()->get(i).asDouble();
    mutex.post();
    return true;
}

yarp::dev::IRangefinder2D::Device_status Rangefinder2DInputPortProcessor::getStatus()
{
    mutex.wait();
    yarp::dev::IRangefinder2D::Device_status status = (yarp::dev::IRangefinder2D::Device_status) lastBottle.get(3).asInt();
    mutex.post();
    return status;
}

inline int Rangefinder2DInputPortProcessor::getIterations()
{
    mutex.wait();
    int ret=count;
    mutex.post();
    return ret;
}

// time is in ms
void Rangefinder2DInputPortProcessor::getEstFrequency(int &ite, double &av, double &min, double &max)
{
    mutex.wait();
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
    mutex.post();
}

bool yarp::dev::Rangefinder2DClient::open(yarp::os::Searchable &config)
{
    local.clear();
    remote.clear();

    local  = config.find("local").asString().c_str();
    remote = config.find("remote").asString().c_str();

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

    if (config.check("period"))
    {
        _rate = config.find("period").asInt();
    }
    else
    {
        yError("Rangefinder2DClient::open() missing period parameter");
        return false;
    }

    ConstString local_rpc = local;
    local_rpc += "/rpc:o";
    ConstString remote_rpc = remote;
    remote_rpc += "/rpc:i";

    if (!inputPort.open(local.c_str()))
    {
        yError("Rangefinder2DClient::open() error could not open port %s, check network\n",local.c_str());
        return false;
    }
    inputPort.useCallback();

    if (!rpcPort.open(local_rpc.c_str()))
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

    ok=Network::connect(local_rpc.c_str(), remote_rpc.c_str());
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
    yarp::dev::PolyDriver* drv = new yarp::dev::PolyDriver;
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
        bool b_canOpenTransformClient = drv->open(TransformClientOptions);
    }

    if (b_canOpenTransformClient)
    {
        yarp::dev::IFrameTransform* iTrf = 0;
        drv->view(iTrf);
        if (iTrf != 0)
        {
            yError() << "A Problem occurred while trying to view the IFrameTransform interface";
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
            device_position_x = config.find("device_position_x").asDouble();
            device_position_y = config.find("device_position_y").asDouble();
            device_position_theta = config.find("device_position_theta").asDouble();
        }
        else
        {
            yDebug() << "No position information provided for this device";
        }
    }


    return true;
}

bool yarp::dev::Rangefinder2DClient::close()
{
    rpcPort.close();
    inputPort.close();
    return true;
}

bool yarp::dev::Rangefinder2DClient::getRawMeasurementData(yarp::sig::Vector &data)
{
    inputPort.getData(data);
    return true;
}

bool yarp::dev::Rangefinder2DClient::getCartesianMeasurementData(std::vector<CartesianMeasurementData> &data)
{
    yarp::sig::Vector ranges;
    inputPort.getData(ranges);
    size_t size = ranges.size();
    data.resize(size);
    double laser_angle_of_view = fabs(scan_angle_min) + fabs(scan_angle_max);
    for (size_t i = 0; i < size; i++)
    {
        double angle = (i / double(size)*laser_angle_of_view + device_position_theta + scan_angle_min)* DEG2RAD;
        data[i].x = ranges[i] * cos(angle) + device_position_x;
        data[i].y = ranges[i] * sin(angle) + device_position_y;
    }
    return true;
}

bool yarp::dev::Rangefinder2DClient::getPolarMeasurementData(std::vector<PolarMeasurementData> &data)
{
    yarp::sig::Vector ranges;
    inputPort.getData(ranges);
    size_t size = ranges.size();
    data.resize(size);
    double laser_angle_of_view = fabs(scan_angle_min) + fabs(scan_angle_max);
    for (size_t i = 0; i < size; i++)
    {
        double angle = (i / double(size)*laser_angle_of_view + scan_angle_min)* DEG2RAD;
        data[i].angle = angle;
        data[i].distance = ranges[i];
    }
    return true;
}

bool yarp::dev::Rangefinder2DClient::getDistanceRange(double& min, double& max)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_DISTANCE_RANGE);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        min = response.get(2).asDouble();
        max = response.get(3).asDouble();
        return true;
    }
    return false;
}

bool yarp::dev::Rangefinder2DClient::setDistanceRange(double min, double max)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_DISTANCE_RANGE);
    cmd.addDouble(min);
    cmd.addDouble(max);
    bool ok = rpcPort.write(cmd, response);
    if (ok)
    {
        scan_angle_min = min;
        scan_angle_max = max;
    }
    return (CHECK_FAIL(ok, response));
}

bool yarp::dev::Rangefinder2DClient::getScanLimits(double& min, double& max)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_ANGULAR_RANGE);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        min = scan_angle_min = response.get(2).asDouble();
        max = scan_angle_max = response.get(3).asDouble();
        return true;
    }
    return false;
}

bool yarp::dev::Rangefinder2DClient::setScanLimits(double min, double max)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_ANGULAR_RANGE);
    cmd.addDouble(min);
    cmd.addDouble(max);
    bool ok = rpcPort.write(cmd, response);
    return (CHECK_FAIL(ok, response));
}

bool yarp::dev::Rangefinder2DClient::getHorizontalResolution(double& step)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_ANGULAR_STEP);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        step = response.get(2).asDouble();
        return true;
    }
    return false;
}

bool yarp::dev::Rangefinder2DClient::setHorizontalResolution(double step)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_ANGULAR_STEP);
    cmd.addDouble(step);
    bool ok = rpcPort.write(cmd, response);
    return (CHECK_FAIL(ok, response));
}

bool yarp::dev::Rangefinder2DClient::getScanRate(double& rate)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_SCAN_RATE);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        rate = response.get(2).asDouble();
        return true;
    }
    return false;
}

bool yarp::dev::Rangefinder2DClient::setScanRate(double rate)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_SCAN_RATE);
    cmd.addDouble(rate);
    bool ok = rpcPort.write(cmd, response);
    return (CHECK_FAIL(ok, response));
}

bool yarp::dev::Rangefinder2DClient::getDeviceStatus(Device_status &status)
{
    status = inputPort.getStatus();
    return true;
}

bool yarp::dev::Rangefinder2DClient::getDeviceInfo(yarp::os::ConstString &device_info)
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

Stamp yarp::dev::Rangefinder2DClient::getLastInputStamp()
{
    return lastTs;
}

yarp::dev::DriverCreator *createRangefinder2DClient() {
    return new DriverCreatorOf<Rangefinder2DClient>("Rangefinder2DClient",
        "",
        "Rangefinder2DClient");
}

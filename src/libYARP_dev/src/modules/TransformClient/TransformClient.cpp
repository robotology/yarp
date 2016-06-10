/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include <TransformClient.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

/*! \file TransformClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


inline void TransformInputPortProcessor::resetStat()
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

TransformInputPortProcessor::TransformInputPortProcessor()
{
    //state=::TRANSFORM_GENERAL_ERROR;
    resetStat();
}

void TransformInputPortProcessor::onRead(yarp::os::Bottle &b)
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
        /*if (tmpDT*1000<TRANSFORM_TIMEOUT)
        {
            state = b.get(5).asInt();
        }
        else
        {
            state = TRANSFORM_TIMEOUT;
        }*/
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
    if ((1000*(newStamp.getTime()-lastStamp.getTime()))<TRANSFORM_TIMEOUT_MS)
    {
        state = b.get(5).asInt();
    }
    else
    {
        state = ITransform::TRANSFORM_TIMEOUT;
    }
    lastStamp = newStamp;

    mutex.post();
}

inline int TransformInputPortProcessor::getLast(yarp::os::Bottle &data, Stamp &stmp)
{
    mutex.wait();
    int ret=state;
    if (ret != ITransform::TRANSFORM_GENERAL_ERROR)
    {
        data=lastBottle;
        stmp = lastStamp;
    }
    mutex.post();

    return ret;
}

int    TransformInputPortProcessor::getStatus()
{
    mutex.wait();
    int status = lastBottle.get(3).asInt();
    mutex.post();
    return status;
}


inline int TransformInputPortProcessor::getIterations()
{
    mutex.wait();
    int ret=count;
    mutex.post();
    return ret;
}

// time is in ms
void TransformInputPortProcessor::getEstFrequency(int &ite, double &av, double &min, double &max)
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

bool yarp::dev::TransformClient::open(yarp::os::Searchable &config)
{
    local.clear();
    remote.clear();

    local  = config.find("local").asString().c_str();
    remote = config.find("remote").asString().c_str();

    if (local=="")
    {
        yError("TransformClient::open() error you have to provide valid local name");
        return false;
    }
    if (remote=="")
    {
        yError("TransformClient::open() error you have to provide valid remote name");
        return false;
    }

    if (config.check("period"))
    {
        _rate = config.find("period").asInt();
    }
    else
    {
        yError("BatteryClient::open() missing period parameter");
        return false;
    }

    ConstString local_rpc = local;
    local_rpc += "/rpc:o";
    ConstString remote_rpc = remote;
    remote_rpc += "/rpc:i";

    if (!inputPort.open(local.c_str()))
    {
        yError("TransformClient::open() error could not open port %s, check network",local.c_str());
        return false;
    }
    inputPort.useCallback();

    if (!rpcPort.open(local_rpc.c_str()))
    {
        yError("TransformClient::open() error could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    bool ok=Network::connect(remote.c_str(), local.c_str(), "udp");
    if (!ok)
    {
        yError("TransformClient::open() error could not connect to %s", remote.c_str());
        return false;
    }

    ok=Network::connect(local_rpc.c_str(), remote_rpc.c_str());
    if (!ok)
    {
        yError("TransformClient::open() error could not connect to %s", remote_rpc.c_str());
        return false;
    }

    return true;
}

bool yarp::dev::TransformClient::close()
{
    rpcPort.close();
    inputPort.close();
    return true;
}

std::string yarp::dev::TransformClient::allFramesAsString()
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::canTransform(const std::string &target_frame, const std::string &source_frame, std::string *error_msg)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::clear()
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::frameExists(const std::string &frame_id)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::getAllFrameIds(std::vector< std::string > &ids)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::getParent(const std::string &frame_id, std::string &parent_frame_id)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::getTransform(const std::string &target_frame_id, const std::string &source_frame_id, yarp::sig::Matrix &transform)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::setTransform(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Matrix &transform)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::deleteTransform(const std::string &target_frame_id, const std::string &source_frame_id)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::transformPoint(const std::string &target_frame_id, const yarp::sig::Vector &input_point, yarp::sig::Vector &transformed_point)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::transformPose(const std::string &target_frame_id, const yarp::sig::Vector &input_pose, yarp::sig::Vector &transformed_pose)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::transformQuaternion(const std::string &target_frame_id, const yarp::sig::Vector &input_quaternion, yarp::sig::Vector &transformed_quaternion)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::waitForTransform(const std::string &target_frame_id, const std::string &source_frame_id, const double &timeout)
{
    yError() << "Not yet implemented"; return false;
}

/*
bool yarp::dev::TransformClient::getBatteryTemperature(double &temperature)
{
    temperature = inputPort.getStatus();
    return true;
}
*/
/*
bool yarp::dev::TransformClient::getInfo(yarp::os::ConstString &info)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_IBATTERY);
    cmd.addVocab(VOCAB_BATTERY_INFO);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response)!=false)
    {
        battery_info = response.get(2).asString();
        return true;
    }
    return false;
}
*/
/*
Stamp yarp::dev::TransformClient::getLastInputStamp()
{
    return lastTs;
}*/

yarp::dev::DriverCreator *createTransformClient() {
    return new DriverCreatorOf<TransformClient>("transformClient",
        "",
        "transformClient");
}

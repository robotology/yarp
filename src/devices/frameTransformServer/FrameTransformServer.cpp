/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

// example: yarpdev --device transformServer --ROS::enable_ros_publisher 0 --ROS::enable_ros_subscriber 0

#define _USE_MATH_DEFINES
#include "FrameTransformServer.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/dev/ControlBoardInterfaces.h>

#include <cmath>
#include <cstdlib>
#include <limits>
#include <mutex>
#include <sstream>

using namespace yarp::sig;
using namespace yarp::math;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;


namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORMSERVER, "yarp.device.FrameTransformServer")
}

/**
  * FrameTransformServer
  */

FrameTransformServer::FrameTransformServer() : PeriodicThread(DEFAULT_THREAD_PERIOD)
{
    m_period = DEFAULT_THREAD_PERIOD;
    m_FrameTransformTimeout = 0.200; //ms
}

FrameTransformServer::~FrameTransformServer()
{
    threadRelease();
}

bool FrameTransformServer::read(yarp::os::ConnectionReader& connection)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    string request = in.get(0).asString();

    yCError(FRAMETRANSFORMSERVER, "Invalid vocab received");
    out.clear();
    out.addVocab32(VOCAB_ERR);

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        out.write(*returnToSender);
    }
    else
    {
        yCError(FRAMETRANSFORMSERVER) << "Invalid return to sender";
    }
    return true;
}

bool FrameTransformServer::threadInit()
{
    //open rpc port
    if (!m_rpcPort.open(m_rpcPortName))
    {
        yCError(FRAMETRANSFORMSERVER, "Failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    // open data port
    if (!m_streamingPort.open(m_streamingPortName))
    {
        yCError(FRAMETRANSFORMSERVER, "Failed to open port %s", m_streamingPortName.c_str());
        return false;
    }

    yCInfo(FRAMETRANSFORMSERVER) << "Transform server started";
    return true;
}

bool FrameTransformServer::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString());

    if (!config.check("period"))
    {
        m_period = 0.01;
    }
    else
    {
        m_period = config.find("period").asInt32() / 1000.0;
        yCInfo(FRAMETRANSFORMSERVER) << "Thread period set to:" << m_period;
    }

    if (config.check("transforms_lifetime"))
    {
        m_FrameTransformTimeout = config.find("transforms_lifetime").asFloat64();
        yCInfo(FRAMETRANSFORMSERVER) << "transforms_lifetime set to:" << m_FrameTransformTimeout;
    }

    std::string name;
    if (!config.check("name"))
    {
        name = "transformServer";
    }
    else
    {
        name = config.find("name").asString();
    }
    m_streamingPortName =  "/"+ name + "/transforms:o";
    m_rpcPortName = "/" + name + "/rpc";

    this->start();

    yarp::os::Time::delay(0.5);

    return true;
}

void FrameTransformServer::threadRelease()
{
    m_streamingPort.interrupt();
    m_streamingPort.close();
    m_rpcPort.interrupt();
    m_rpcPort.close();
}

void FrameTransformServer::run()
{
    std::lock_guard<std::mutex> lock(m_mutex);
}

bool FrameTransformServer::close()
{
    yCTrace(FRAMETRANSFORMSERVER, "Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    return true;
}

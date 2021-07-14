/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FrameTransformServer.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/math/Math.h>

#include <cmrc/cmrc.hpp>
#include <mutex>
CMRC_DECLARE(frameTransformServerRC);

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;
using namespace std;

namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORMSERVER, "yarp.device.FrameTransformServer")
}
#define LOG_THROTTLE_PERIOD 1.0

//------------------------------------------------------------------------------------------------------------------------------
bool FrameTransformServer::read(yarp::os::ConnectionReader& connection)
{
    std::lock_guard<std::mutex> lock (m_rpc_mutex);
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) {
        return false;
    }

    string request = in.get(0).asString();
    if (request == "help")
    {
        out.addVocab32("many");
        out.addString("No RPC commands available");
    }
    else
    {
        yCError(FRAMETRANSFORMSERVER, "Invalid vocab received in FrameTransformServer");
        out.clear();
        out.addVocab32(VOCAB_ERR);
        out.addString("Invalid command name");
    }

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

bool FrameTransformServer::open(yarp::os::Searchable &config)
{
    yCWarning(FRAMETRANSFORMSERVER) << "The 'FrameTransformServer' device is experimental and could be modified without any warning";

    yarp::os::Property cfg;
    cfg.fromString(config.toString());

    string configuration_to_open;
    string innerFilePath="config_xml/fts_yarp_only.xml";
    auto fs = cmrc::frameTransformServerRC::get_filesystem();
    if(cfg.check("filexml_option")) { innerFilePath="config_xml/"+cfg.find("filexml_option").toString();}
    cfg.unput("filexml_option");
    auto xmlFile = fs.open(innerFilePath);
    for(const auto& lemma : xmlFile)
    {
        configuration_to_open += lemma;
    }

    string m_local_rpcUser = "/ftServer/rpc";
    if (cfg.check("local_rpc")) { m_local_rpcUser=cfg.find("local_rpc").toString();}
    cfg.unput("local_rpc");

    yarp::robotinterface::XMLReader reader;
    yarp::robotinterface::XMLReaderResult result = reader.getRobotFromString(configuration_to_open, cfg);
    yCAssert(FRAMETRANSFORMSERVER, result.parsingIsSuccessful);

    m_robot = std::move(result.robot);

    if (!m_robot.enterPhase(yarp::robotinterface::ActionPhaseStartup)) {
        return false;
    }

    if (!m_robot.enterPhase(yarp::robotinterface::ActionPhaseRun)) {
        return false;
    }

    if (!m_rpc_InterfaceToUser.open(m_local_rpcUser))
    {
        yCError(FRAMETRANSFORMSERVER,"Failed to open rpc port");
    }

    m_rpc_InterfaceToUser.setReader(*this);

    return true;
}

bool FrameTransformServer::close()
{
    m_robot.enterPhase(yarp::robotinterface::ActionPhaseInterrupt1);
    m_robot.enterPhase(yarp::robotinterface::ActionPhaseShutdown);
    m_rpc_InterfaceToUser.close();
    return true;
}

FrameTransformServer::FrameTransformServer() : PeriodicThread(0.01),
    m_period(0.01)
{
}

FrameTransformServer::~FrameTransformServer() = default;

bool     FrameTransformServer::threadInit()
{
    yCTrace(FRAMETRANSFORMSERVER, "Thread started");
    return true;
}

void     FrameTransformServer::threadRelease()
{
    yCTrace(FRAMETRANSFORMSERVER, "Thread stopped");
}

void     FrameTransformServer::run()
{
    //Empty (placeholder for future developments)
}

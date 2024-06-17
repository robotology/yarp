/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "GstreamerCarrier.h"
#include "GstreamerStream.h"

#include <yarp/os/ConnectionState.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Name.h>
#include <yarp/os/Route.h>
#include <yarp/os/impl/FakeFace.h>

#include <string>

using namespace yarp::os;

YARP_LOG_COMPONENT(GSTREAMER_CARRIER,
                   "yarp.carrier.gstreamer.gstreamerCarrier",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)

GstreamerCarrier::GstreamerCarrier() = default;

yarp::os::Carrier* GstreamerCarrier::create() const
{
    return new GstreamerCarrier();
}

std::string GstreamerCarrier::getName() const
{
    return "gstreamer";
}

bool GstreamerCarrier::checkHeader(const Bytes& header)
{
    return true;
}

void GstreamerCarrier::getHeader(Bytes& header) const
{
}

void GstreamerCarrier::setParameters(const Bytes& header)
{
    YARP_UNUSED(header);
}

bool GstreamerCarrier::isConnectionless() const
{
    return true;
}


bool GstreamerCarrier::respondToHeader(ConnectionState& proto)
{
    auto* stream = new GstreamerStream();
    yAssert(stream != nullptr);

    Contact remote = proto.getStreams().getRemoteAddress();
    bool ok = stream->open(this->pipeline_string,remote);
    if (!ok) {
        delete stream;
        return false;
    }

    int myPort = stream->getLocalAddress().getPort();
    proto.takeStreams(stream);

    return true;
}

bool GstreamerCarrier::expectReplyToHeader(ConnectionState& proto)
{
    auto* stream = new GstreamerStream();
    if (stream == nullptr) {
        return false;
    }

    yarp::os::Contact remote = proto.getStreams().getRemoteAddress();
    bool ok = stream->open(this->pipeline_string,remote);

    // std::cout << "Remote contact info: host=" << proto.getRoute().getToContact().getHost() << " port= " << proto.getRoute().getToContact().getPort() <<std::endl;
    if (!ok) {
        delete stream;
        return false;
    }

    proto.takeStreams(stream);
    return true;
}

/////////////////////
bool GstreamerCarrier::prepareSend(ConnectionState& proto)
{
    // get all parameters of this carrier
    yarp::os::Name n(proto.getRoute().getCarrierName() + "://test");

    // default pipeline
    pipeline_string = "udpsrc port = 15001 caps = \"application/x-rtp, media = (string)video, encoding-name = (string)H264, payload = (int)96 \" ! rtph264depay ! h264parse ! avdec_h264 ";

    bool has_pipeline_field = false;
    bool has_pipeline_env = false;

    //check for pipelineCmd option
    //BEWARE: The following code is not usable, since yarp connect does not currently support
    //spaces in the command line. Maybe in the future...
    {
        std::string tmp_pipeline_string = n.getCarrierModifier("pipelineCmd", &has_pipeline_field);

        if (has_pipeline_field)
        {
            pipeline_string = tmp_pipeline_string;
            yCDebug(GSTREAMER_CARRIER, "Using pipeline from command line: %s", pipeline_string.c_str());
        }
    }

    //check for pipelineEnv option
    {
        std::string tmp_pipeline_env_string = n.getCarrierModifier("pipelineEnv", &has_pipeline_env);

        if (has_pipeline_env) {
            const char* env_p = std::getenv(tmp_pipeline_env_string.c_str());
            if (env_p!=nullptr)
            {
                pipeline_string = env_p;
                yCDebug(GSTREAMER_CARRIER, "Using pipeline from env: %s", pipeline_string.c_str());
            }
        }
    }

    if (has_pipeline_field == false &&
        has_pipeline_env == false)
    {
        yCWarning(GSTREAMER_CARRIER, "pipeline parameter not found! Using default test string");
    }

    return true;
}

bool GstreamerCarrier::sendHeader(ConnectionState& proto)
{
    return true;
}

bool GstreamerCarrier::expectSenderSpecifier(ConnectionState& proto)
{
    return true;
}

bool GstreamerCarrier::expectExtraHeader(ConnectionState& proto)
{
    return true;
}

yarp::os::Face* GstreamerCarrier::createFace() const
{
    return new yarp::os::impl::FakeFace();
}

bool GstreamerCarrier::expectAck(ConnectionState& proto)
{
    return true;
}

bool GstreamerCarrier::write(ConnectionState& proto, SizedWriter& writer)
{
    return false;
}

std::string GstreamerCarrier::toString() const
{
    return "gstreamer_carrier";
}

bool GstreamerCarrier::expectIndex(ConnectionState& proto)
{
    return true;
}

bool GstreamerCarrier::sendAck(ConnectionState& proto)
{
    return true;
}

bool GstreamerCarrier::reply(ConnectionState& proto, SizedWriter& writer)
{
    return false;
}

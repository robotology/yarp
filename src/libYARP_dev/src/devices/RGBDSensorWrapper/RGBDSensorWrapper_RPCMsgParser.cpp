/*
* Copyright (C) 2016 RobotCub Consortium
* Author: Alberto Cardellino
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include <yarp/os/LogStream.h>
#include "RGBDSensorWrapper_RPCMsgParser.h"

using namespace sensor::depth;
using namespace yarp::dev;

RGBDSensor_RPCMgsParser::RGBDSensor_RPCMgsParser()
{
    yTrace();
}

RGBDSensor_RPCMgsParser::~RGBDSensor_RPCMgsParser()
{
    yTrace();
}

bool RGBDSensor_RPCMgsParser::respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
    // TBD: TO BE DONE!!
    if(verbose >= 5)
        yTrace() << "\nRGBDSensor_RPCMgsParser Received command:\n\t " << command.toString();

    bool ok  = false;   // command succesfull
//     bool rec = false;   // command recognized -- maybe useless here

    switch(command.get(0).asInt())
    {
        case 0:
            reply.clear();
            reply.addString("Reply to command: " + command.toString());
            ok = true;
        break;

        default:
            ok = yarp::dev::DeviceResponder::respond(command, reply);
    }
    return ok;
}

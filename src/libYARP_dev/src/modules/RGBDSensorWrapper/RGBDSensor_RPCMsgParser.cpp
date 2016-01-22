

#include <RGBDSensor_RPCMsgParser.h>
#include <yarp/os/LogStream.h>

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
    if(verbose >= IRGBDSensor::VERY_VERBOSE)
        yTrace() << "\nRGBDSensor_RPCMgsParser Received command:\n\t " << command.toString();
    return yarp::dev::DeviceResponder::respond(command, reply);
}

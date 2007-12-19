// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>

using namespace yarp::os;
using namespace yarp::dev;

/** Lorenzo Natale, Dec 2007. Test remotization of calibrate 
 *  functions. Useful for general tests on device remotization 
 *  (see fakebot for example).
 */
int main(int argc, const char **argv)
{
    Network::init();
    IControlCalibration2 *ical;

    Property p;
    p.put("device", "remote_controlboard");
    p.put("local", "/motortest");     //prefix for local names
    p.put("remote", "/controlboard"); //prefix for remote names

    PolyDriver device;
    device.open(p);
    device.view(ical);

    ical->calibrate();
    ical->calibrate2(1,1000,1.1,1.1,1.1);
    ical->done(2);
    ical->park();

    device.close();
    Network::fini();
    return 0; 
}


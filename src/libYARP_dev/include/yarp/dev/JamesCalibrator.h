// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __JAMES_CALIBRATOR__
#define __JAMES_CALIBRATOR__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/CalibratorInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>

namespace yarp {
    namespace dev
    {
        class JamesCalibrator;
    }
}

class yarp::dev::JamesCalibrator:public ICalibrator, public DeviceDriver
{
public:
    JamesCalibrator();
    ~JamesCalibrator();

    bool calibrate(DeviceDriver *dd);

    //no open or close methods? check this

private:
    bool calibrateJoint(int j);
    void goToZero(int j);

    IControlCalibration *iCalibrate;
    IAmplifierControl *iAmps;
    IPidControl *iPids;
    IEncoders *iEncoders;
    IPositionControl *iPosition;
};
#endif

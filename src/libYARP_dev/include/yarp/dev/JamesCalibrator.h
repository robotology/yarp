// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __JAMES_CALIBRATOR__
#define __JAMES_CALIBRATOR__

#include <yarp/dev/CalibratorInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>

namespace yarp {
    namespace dev
    {
        class JamesCalibrator;
    }
}

class yarp::dev::JamesCalibrator:public ICalibrator
{
public:
    JamesCalibrator();
    ~JamesCalibrator();

    bool calibrate(DeviceDriver *dd);

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

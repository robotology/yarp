// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __JAMES_CALIBRATOR__
#define __JAMES_CALIBRATOR__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/CalibratorInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>

/*

YYYCalibrator--->ICalibrator

XXXMotorControl--->IControlCalibrationRaw
IControlCalibration

Do:  Create XXXMotorControl
Create YYYCalibrator
IControlCalibrationRaw->setCalibrator(ICalibrator *)
IControlCalibrationRaw->calibrate() which calls ICalibrator->calibrate(XXXMotionControl*)
IControlCalibration methods are called by YYYCalibrator to perform the calibration
*/

namespace yarp {
    namespace dev
    {
        class JamesArmCalibrator;
    }
}

/**
 * \file JamesArmCalibrator.h 
 * A device driver to implement the calibration of the arm of James.
 */

/**
 * @ingroup dev_impl
 * 
 * A calibrator interface implementation for the arm of the robot James.
 */
class yarp::dev::JamesArmCalibrator : public ICalibrator, public DeviceDriver
{
public:
    /**
     * Default constructor.
     */
    JamesArmCalibrator();
    
    /**
     * Destructor.
     */
    ~JamesArmCalibrator();

    /**
     * Calibrate method. Call this to calibrate the complete device.
     * @param dd is a pointer to the DeviceDriver to calibrate which must implement
     * the position control interfaces of the standard control board devices.
     * @return true if calibration was successful, false otherwise.
     */
    bool calibrate(DeviceDriver *dd);

    /**
     * Open the device driver.
     * @param config is a reference to a Searchable object which contains the initialization
     * parameters.
     * @return true/false on success/failure.
     */
	virtual bool open (yarp::os::Searchable& config);

    /**
     * Close the device driver.
     * @return true/false on success/failure.
     */
	virtual bool close ();

private:
    bool calibrateJoint(int j);
    void goToZero(int j);

    IControlCalibration *iCalibrate;
    IAmplifierControl *iAmps;
    IPidControl *iPids;
    IEncoders *iEncoders;
    IPositionControl *iPosition;

	double *param;
	double *pos;
	double *vel;
};
#endif

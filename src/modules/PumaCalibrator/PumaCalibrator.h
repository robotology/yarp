// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2007 Mattia Castelnovi
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#ifndef __PUMA_CALIBRATOR__
#define __PUMA_CALIBRATOR__

//#include <yarp/dev/DeviceDriver.h>
//#include <yarp/dev/CalibratorInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>




//#include <yarp/dev/PolyDriver.h>
//#include <yarp/dev/ControlBoardInterfacesImpl.h>



//using namespace yarp::os;
//using namespace yarp::dev;



namespace yarp {
    namespace dev
    {
        class PumaCalibrator;
    }
}

/**
 * \file PumaCalibrator.h
 * A device driver to implement the calibration of the  Puma Robot Arm
 */

/**
 * @ingroup dev_impl_motor
 *
 * A calibrator interface implementation for the Puma Robot Arm
 */
class yarp::dev::PumaCalibrator : public ICalibrator, public DeviceDriver
{
public:
    /**
     * Default constructor.
     */
    PumaCalibrator();

    /**
     * Destructor.
     */
    ~PumaCalibrator();

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
    bool open (yarp::os::Searchable& config);

    /**
     * Close the device driver.
     * @return true/false on success/failure.
     */
    bool close ();

     bool park(DeviceDriver *dd, bool wait=true);
      bool quitCalibrate();
      bool quitPark();

private:
    bool calibrateJoint(int j);
    void goToZero(int j);

    IControlCalibration *iCalibrate;
    IAmplifierControl   *iAmps;
    IPidControl         *iPids;
    IEncoders           *iEncoders;
    IPositionControl    *iPosition;
    unsigned char *type;
    double *speed1;
    double *accs;
    double *PositionZero;
    double *pos;
    double *vel;

};
#endif

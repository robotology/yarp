/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __FAKE_CALIBRATOR__
#define __FAKE_CALIBRATOR__

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/CalibratorInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>

#include "FakeCalibrator_ParamsParser.h"


/**
 * @ingroup dev_impl_fake
 * @brief `fakeCalibrator`: A fake calibrator implementation for testing purposes.
 * 
 * Parameters required by this device are shown in class: FakeCalibrator_ParamsParser
 */
class FakeCalibrator : public yarp::dev::ICalibrator,
                       public yarp::dev::DeviceDriver,
                       public yarp::dev::IRemoteCalibrator,
                       public FakeCalibrator_ParamsParser
{
public:
    /**
     * Default constructor.
     */
    FakeCalibrator();

    /**
     * Destructor.
     */
    ~FakeCalibrator();

    /**
     * Open the device driver.
     * @param config is a reference to a Searchable object which contains the initialization
     * parameters.
     * @return true/false on success/failure.
     */
    virtual bool open (yarp::os::Searchable& config) override;

    /**
     * Close the device driver.
     * @return true/false on success/failure.
     */
    virtual bool close ()  override;

    // ICalibrator

    /**
     * Calibrate method. Call this to calibrate the complete device.
     * @param dd is a pointer to the DeviceDriver to calibrate which must implement
     * the position control interfaces of the standard control board devices.
     * @return true if calibration was successful, false otherwise.
     */
    yarp::dev::ReturnValue calibrate(DeviceDriver *device) override;
    yarp::dev::ReturnValue park(DeviceDriver *dd, bool wait=true)  override;
    yarp::dev::ReturnValue quitPark()  override;
    yarp::dev::ReturnValue quitCalibrate()  override;

    // IRemoteCalibrator
    yarp::dev::IRemoteCalibrator *getCalibratorDevice()  override;
    yarp::dev::ReturnValue calibrateSingleJoint(int j)  override;
    yarp::dev::ReturnValue calibrateWholePart()  override;
    yarp::dev::ReturnValue homingSingleJoint(int j)  override;
    yarp::dev::ReturnValue homingWholePart()  override;
    yarp::dev::ReturnValue parkSingleJoint(int j, bool _wait=true)  override;
    yarp::dev::ReturnValue parkWholePart()  override;

private:
    yarp::dev::PolyDriver*            m_dev2calibrate = nullptr;
    yarp::dev::IControlCalibration*   m_iControlCalibration = nullptr;
    yarp::dev::IEncoders*             m_iEncoders = nullptr;
    yarp::dev::CalibrationParameters  m_cparams;
};

#endif

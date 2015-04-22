// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARP2_CALLIBRATOR_INTERFACES__
#define __YARP2_CALLIBRATOR_INTERFACES__

#include <yarp/dev/DeviceDriver.h>

namespace yarp {
    namespace dev {
        class ICalibrator;
        class IRemoteCalibrator;
    }
}

class YARP_dev_API yarp::dev::ICalibrator
{
public:
    virtual ~ICalibrator(){}

    virtual bool calibrate(DeviceDriver *dd)=0;

    virtual bool park(DeviceDriver *dd, bool wait=true)=0;

    virtual bool quitCalibrate()=0;

    virtual bool quitPark()=0;
};

/**
 * IRemoteCalibrator interface is meant to remotize the access of the calibration device
 * in order to allow users to remotely call the calibration procedure for
 * a single joint or the whole device and let the calibrator do the job.
 * Main difference between this interface and remotizing the iControlCalibration
 * interfaces is that in this case we are calling a proper iCalibrator device
 * (whose interface is described above) that knows the correct procedure and
 * the correct parameters for the calibration, so that user doesn't have to worry
 * about configuration.
 * This is meant to be used by network wrapper devices
 */

class YARP_dev_API yarp::dev::IRemoteCalibrator
{
private:
    yarp::dev::IRemoteCalibrator   *_remoteCalibrator;

public:
    IRemoteCalibrator();

    virtual ~IRemoteCalibrator(){}

    /**
     * @brief setCalibratorDevice: store the pointer to the calibrator device. Has a default
     *          implementation and it is meant to be called only by wrapper devices attached
     *          to the calibrator.
     *          It will be not used by remote device like remoteControlBoard.
     * @param dev: device implementing the IRemotizableCalibrator interface
     * @return true on success
     */

    virtual bool setCalibratorDevice(yarp::dev::IRemoteCalibrator *dev);

    /**
     * @brief getCalibratorDevice: return the pointer stored with the setCalibratorDevice
     * @return yarp::dev::IRemotizableCalibrator pointer or NULL if not valid.
     */
    virtual yarp::dev::IRemoteCalibrator *getCalibratorDevice();

    /**
     * @brief calibrateSingleJoint: call the calibration procedure for the single joint
     * @param j: joint to be calibrated
     * @return true if calibration was successful
     */
    virtual bool calibrateSingleJoint(int j)=0;

    /**
     * @brief calibrateWholePart: call the procedure for calibrating the whole device
     * @return true if calibration was successful
     */
    virtual bool calibrateWholePart()=0;

    /**
     * @brief homingSingleJoint: call the homing procedure for a single joint
     * @param j: joint to be calibrated
     * @return true if homing was succesful, false otherwise
     */
    virtual bool homingSingleJoint(int j)=0;

    /**
     * @brief homingWholePart: call the homing procedure for a the whole part/device
     * @return true if homing was succesful, false otherwise
     */
    virtual bool homingWholePart()=0;

    /**
     * @brief parkSingleJoint(): start the parking procedure for the single joint
     * @return true if succesful
     */
    virtual bool parkSingleJoint(int j, bool _wait=true)=0;

    /**
     * @brief parkWholePart: start the parking procedure for the whole part
     * @return true if succesful
     */
    virtual bool parkWholePart()=0;

    /**
     * @brief quitCalibrate: interrupt the calibration procedure
     * @return true if succesful
     */
    virtual bool quitCalibrate()=0;

    /**
     * @brief quitPark: interrupt the park procedure
     * @return true if succesful
     */
    virtual bool quitPark()=0;
};

#define VOCAB_REMOTE_CALIBRATOR_INTERFACE           VOCAB4('r','e','c','a')

#define VOCAB_CALIBRATE_SINGLE_JOINT                VOCAB3('c','a','l')
#define VOCAB_CALIBRATE_WHOLE_PART                  VOCAB4('c','a','l','s')
#define VOCAB_HOMING_SINGLE_JOINT                   VOCAB3('h','o','m')
#define VOCAB_HOMING_WHOLE_PART                     VOCAB4('h','o','m','s')
#define VOCAB_PARK_SINGLE_JOINT                     VOCAB3('p','a','r')
#define VOCAB_PARK_WHOLE_PART                       VOCAB4('p','a','r','s')
#define VOCAB_QUIT_CALIBRATE                        VOCAB4('q','u','c','a')
#define VOCAB_QUIT_PARK                             VOCAB4('q','u','p','a')

#endif

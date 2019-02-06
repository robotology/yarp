/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CALIBRATORINTERFACES_H
#define YARP_DEV_CALIBRATORINTERFACES_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/Vocab.h>

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
     * @brief isCalibratorDevicePresent: check if a calibrator device has been set
     * @return true if a valid calibrator device has been found
     */
    virtual bool isCalibratorDevicePresent(bool *isCalib);

    /**
     * @brief releaseCalibratorDevice: reset the internal pointer to NULL when stop using the calibrator
     */
    virtual void releaseCalibratorDevice(void);

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
     * @return true if homing was successful, false otherwise
     */
    virtual bool homingSingleJoint(int j)=0;

    /**
     * @brief homingWholePart: call the homing procedure for a the whole part/device
     * @return true if homing was successful, false otherwise
     */
    virtual bool homingWholePart()=0;

    /**
     * @brief parkSingleJoint(): start the parking procedure for the single joint
     * @return true if successful
     */
    virtual bool parkSingleJoint(int j, bool _wait=true)=0;

    /**
     * @brief parkWholePart: start the parking procedure for the whole part
     * @return true if successful
     */
    virtual bool parkWholePart()=0;

    /**
     * @brief quitCalibrate: interrupt the calibration procedure
     * @return true if successful
     */
    virtual bool quitCalibrate()=0;

    /**
     * @brief quitPark: interrupt the park procedure
     * @return true if successful
     */
    virtual bool quitPark()=0;
};

constexpr yarp::conf::vocab32_t VOCAB_REMOTE_CALIBRATOR_INTERFACE           = yarp::os::createVocab('r','e','c','a');
constexpr yarp::conf::vocab32_t VOCAB_IS_CALIBRATOR_PRESENT                 = yarp::os::createVocab('i','s','c','a');
constexpr yarp::conf::vocab32_t VOCAB_CALIBRATE_SINGLE_JOINT                = yarp::os::createVocab('c','a','l');
constexpr yarp::conf::vocab32_t VOCAB_CALIBRATE_WHOLE_PART                  = yarp::os::createVocab('c','a','l','s');
constexpr yarp::conf::vocab32_t VOCAB_HOMING_SINGLE_JOINT                   = yarp::os::createVocab('h','o','m');
constexpr yarp::conf::vocab32_t VOCAB_HOMING_WHOLE_PART                     = yarp::os::createVocab('h','o','m','s');
constexpr yarp::conf::vocab32_t VOCAB_PARK_SINGLE_JOINT                     = yarp::os::createVocab('p','a','r');
constexpr yarp::conf::vocab32_t VOCAB_PARK_WHOLE_PART                       = yarp::os::createVocab('p','a','r','s');
constexpr yarp::conf::vocab32_t VOCAB_QUIT_CALIBRATE                        = yarp::os::createVocab('q','u','c','a');
constexpr yarp::conf::vocab32_t VOCAB_QUIT_PARK                             = yarp::os::createVocab('q','u','p','a');

#endif // YARP_DEV_CALIBRATORINTERFACES_H

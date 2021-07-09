/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_REMOTECALIBRATOR_H
#define YARP_DEV_REMOTECALIBRATOR_H

#include <yarp/dev/DeviceDriver.h>

namespace yarp {
namespace dev {

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
class YARP_dev_API IRemoteCalibrator
{
private:
    yarp::dev::IRemoteCalibrator* _remoteCalibrator;

public:
    IRemoteCalibrator();

    virtual ~IRemoteCalibrator() = default;

    /**
     * @brief setCalibratorDevice: store the pointer to the calibrator device. Has a default
     *          implementation and it is meant to be called only by wrapper devices attached
     *          to the calibrator.
     *          It will be not used by remote device like remoteControlBoard.
     * @param dev: device implementing the IRemotizableCalibrator interface
     * @return true on success
     */

    virtual bool setCalibratorDevice(yarp::dev::IRemoteCalibrator* dev);

    /**
     * @brief getCalibratorDevice: return the pointer stored with the setCalibratorDevice
     * @return yarp::dev::IRemotizableCalibrator pointer or NULL if not valid.
     */
    virtual yarp::dev::IRemoteCalibrator* getCalibratorDevice();

    /**
     * @brief isCalibratorDevicePresent: check if a calibrator device has been set
     * @return true if a valid calibrator device has been found
     */
    virtual bool isCalibratorDevicePresent(bool* isCalib);

    /**
     * @brief releaseCalibratorDevice: reset the internal pointer to NULL when stop using the calibrator
     */
    virtual void releaseCalibratorDevice();

    /**
     * @brief calibrateSingleJoint: call the calibration procedure for the single joint
     * @param j: joint to be calibrated
     * @return true if calibration was successful
     */
    virtual bool calibrateSingleJoint(int j) = 0;

    /**
     * @brief calibrateWholePart: call the procedure for calibrating the whole device
     * @return true if calibration was successful
     */
    virtual bool calibrateWholePart() = 0;

    /**
     * @brief homingSingleJoint: call the homing procedure for a single joint
     * @param j: joint to be calibrated
     * @return true if homing was successful, false otherwise
     */
    virtual bool homingSingleJoint(int j) = 0;

    /**
     * @brief homingWholePart: call the homing procedure for a the whole part/device
     * @return true if homing was successful, false otherwise
     */
    virtual bool homingWholePart() = 0;

    /**
     * @brief parkSingleJoint(): start the parking procedure for the single joint
     * @return true if successful
     */
    virtual bool parkSingleJoint(int j, bool _wait = true) = 0;

    /**
     * @brief parkWholePart: start the parking procedure for the whole part
     * @return true if successful
     */
    virtual bool parkWholePart() = 0;

    /**
     * @brief quitCalibrate: interrupt the calibration procedure
     * @return true if successful
     */
    virtual bool quitCalibrate() = 0;

    /**
     * @brief quitPark: interrupt the park procedure
     * @return true if successful
     */
    virtual bool quitPark() = 0;
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_REMOTECALIBRATOR_H

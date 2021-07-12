/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_HAPTICDEVICE_H
#define YARP_DEV_HAPTICDEVICE_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>

/*!
 * \file IHapticDevice.h defines control board standard interfaces
 */

namespace yarp {
    namespace dev {
        class IHapticDevice;
    }
}


/**
 * A generic interface for an haptic device.
 */
class YARP_dev_API yarp::dev::IHapticDevice
{
public:
    virtual ~IHapticDevice() { }

    /*!
     * Get the instantaneous position.
     * @param pos vector containing the returned x-y-z coordinates
     *            expressed in [m].
     * @return true/false on success/failure.
     */
    virtual bool getPosition(yarp::sig::Vector &pos) = 0;

    /*!
     * Get the instantaneous orientation.
     * @param rpy vector containing the returned roll-pitch-yaw
     *            coordinates expressed in [deg].
     * @return true/false on success/failure.
     */
    virtual bool getOrientation(yarp::sig::Vector &rpy) = 0;

    /*!
     * Get the status of the available buttons.
     * @param buttons vector containing the status of each available
     *                button expressed as a double in [0,1].
     * @return true/false on success/failure.
     */
    virtual bool getButtons(yarp::sig::Vector &buttons) = 0;

    /*!
    * Query which feedback mode is active.
    * @param ret true iff Cartesian force feedback is active,
    *            false iff joint torque feedback is active.
    * @return true/false on success/failure.
    */
    virtual bool isCartesianForceModeEnabled(bool &ret) = 0;

    /*!
    * Enable Cartesian force feedback mode.
    * @return true/false on success/failure.
    */
    virtual bool setCartesianForceMode() = 0;

    /*!
    * Enable joint torque feedback mode.
    * @return true/false on success/failure.
    */
    virtual bool setJointTorqueMode() = 0;

    /*!
    * Get maximum values for the feedback.
    * @param max vector containing the 3 maximum bounds
    *            of the feedback.
    *            Units are:
    *            [N] in Cartesian force mode,
    *            [mN*m] in joint torque mode.
    * @return true/false on success/failure.
    */
    virtual bool getMaxFeedback(yarp::sig::Vector &max) = 0;

    /*!
     * Set the values for the force/torque feedback.
     * @param fdbck vector containing the 3 components of the
     *              feedback values.
     *              Units are:
     *              [N] in Cartesian force mode,
     *              [mN*m] in joint torque mode.
     * @return true/false on success/failure.
     */
    virtual bool setFeedback(const yarp::sig::Vector &fdbck) = 0;

    /*!
    * Disable force/torque feedback.
    * @return true/false on success/failure.
    */
    virtual bool stopFeedback() = 0;

    /*!
     * Set the transformation matrix to be applied to position and
     * force feedback data.
     * @param T the 4-by-4 transformation matrix.
     * @return true/false on success/failure.
     */
    virtual bool setTransformation(const yarp::sig::Matrix &T) = 0;

    /*!
     * Get the current transformation matrix used to modify the
     * position readings and force feedback.
     * @param T the returned 4-by-4 transformation matrix.
     * @return true/false on success/failure.
     */
    virtual bool getTransformation(yarp::sig::Matrix &T) = 0;
};

#endif

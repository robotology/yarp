/*
 * SPDX-FileCopyrightText: 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEVICE_FAKE_JOINTCOUPLING
#define YARP_DEVICE_FAKE_JOINTCOUPLING

#include <yarp/os/Time.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IJointCoupling.h>
#include <yarp/dev/ImplementJointCoupling.h>

#include <mutex>


/**
 * @ingroup dev_impl_fake dev_impl_motor
 *
 * \brief `fakeMotionControl`: Documentation to be added
 *
 * The aim of this device is to mimic the expected behavior of a
 * joint coupling device to help testing the high level software.
 *
 * WIP - it is very basic now, not all interfaces are implemented yet.
 */
class FakeJointCoupling :
        public yarp::dev::DeviceDriver,
        public yarp::dev::ImplementJointCoupling
{
private:

public:

    FakeJointCoupling() = default;
    virtual ~FakeJointCoupling() = default;
    // Device Driver
    bool open(yarp::os::Searchable &par) override;
    bool close() override;
    // IJointCoupling
    bool convertFromPhysicalJointsToActuatedAxesPos(const yarp::sig::Vector& physJointsPos, yarp::sig::Vector& actAxesPos) override;
    bool convertFromPhysicalJointsToActuatedAxesVel(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsVel, yarp::sig::Vector& actAxesVel) override;
    bool convertFromPhysicalJointsToActuatedAxesAcc(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsVel, const yarp::sig::Vector& physJointsAcc, yarp::sig::Vector& actAxesAcc) override;
    bool convertFromPhysicalJointsToActuatedAxesTrq(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsTrq, yarp::sig::Vector& actAxesTrq) override;
    bool convertFromActuatedAxesToPhysicalJointsPos(const yarp::sig::Vector& actAxesPos, yarp::sig::Vector& physJointsPos) override;
    bool convertFromActuatedAxesToPhysicalJointsVel(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesVel, yarp::sig::Vector& physJointsVel) override;
    bool convertFromActuatedAxesToPhysicalJointsAcc(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesVel, const yarp::sig::Vector& actAxesAcc, yarp::sig::Vector& physJointsAcc) override;
    bool convertFromActuatedAxesToPhysicalJointsTrq(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesTrq, yarp::sig::Vector& physJointsTrq) override;

};

#endif  // YARP_DEVICE_FAKE_JOINTCOUPLING

/*
 * SPDX-FileCopyrightText: 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IJOINTCOUPLING_H
#define YARP_DEV_IJOINTCOUPLING_H

#include <yarp/dev/api.h>
#include <yarp/os/Vocab.h>
#include <yarp/sig/Vector.h>

namespace yarp::dev {
class IJointCoupling;
}
/**
 * @ingroup dev_iface_motor
 *
 * Interface for a generic control board device implementing position control.
 */
class YARP_dev_API yarp::dev::IJointCoupling
{
public:
    /**
     * Destructor.
     */
    virtual ~IJointCoupling() {}

    virtual bool convertFromPhysicalJointPosToActuatedAxisPos(const yarp::sig::Vector& physJointPos, yarp::sig::Vector& actAxisPos) = 0;
    virtual bool convertFromPhysicalJointVelToActuatedAxisVel(const yarp::sig::Vector& physJointPos, const yarp::sig::Vector& physJointVel, yarp::sig::Vector& actAxisVel) = 0;
    virtual bool convertFromPhysicalJointPosToActuatedAxisAcc(const yarp::sig::Vector& physJointPos, const yarp::sig::Vector& physJointVel, const yarp::sig::Vector& physJointAcc, yarp::sig::Vector& actAxisAcc) = 0;
    virtual bool convertFromPhysicalJointTrqToActuatedAxisTrq(const yarp::sig::Vector& physJointPos, const yarp::sig::Vector& physJointTrq, yarp::sig::Vector& actAxisTrq) = 0;
    virtual bool convertFromActuatedAxisToPhysicalJointPos(const yarp::sig::Vector& actAxisPos, yarp::sig::Vector& physJointPos) = 0;
    virtual bool convertFromActuatedAxisToPhysicalJointVel(const yarp::sig::Vector& actAxisPos, const yarp::sig::Vector& actAxisVel, yarp::sig::Vector& physJointVel) = 0;
    virtual bool convertFromActuatedAxisToPhysicalJointAcc(const yarp::sig::Vector& actAxisPos, const yarp::sig::Vector& actAxisVel, const yarp::sig::Vector& actAxisAcc, yarp::sig::Vector& physJointAcc) = 0;
    virtual bool convertFromActuatedAxisToPhysicalJointTrq(const yarp::sig::Vector& actAxisPos, const yarp::sig::Vector& actAxisTrq, yarp::sig::Vector& physJointTrq) = 0;
    virtual yarp::sig::VectorOf<int> getCoupledJoints()=0;
    virtual std::string getCoupledJointName(int joint)=0;
    virtual bool checkJointIsCoupled(int joint)=0;
    virtual void setCoupledJointLimit(int joint, const double& min, const double& max)=0;
    virtual void getCoupledJointLimit(int joint, double& min, double& max)=0;
};

#endif // YARP_DEV_IJOINTCOUPLING_H

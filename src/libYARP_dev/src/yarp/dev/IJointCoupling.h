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

    virtual bool decouplePos(yarp::sig::Vector& current_pos) = 0;
    virtual bool decoupleVel(yarp::sig::Vector& current_vel) = 0;
    virtual bool decoupleAcc(yarp::sig::Vector& current_acc) = 0;
    virtual bool decoupleTrq(yarp::sig::Vector& current_trq) = 0;
    virtual yarp::sig::Vector decoupleRefPos(yarp::sig::Vector& pos_ref) = 0;
    virtual yarp::sig::Vector decoupleRefVel(yarp::sig::Vector& vel_ref, const yarp::sig::Vector& pos_feedback) = 0;
    virtual yarp::sig::Vector decoupleRefTrq(yarp::sig::Vector& trq_ref) = 0;


    virtual yarp::sig::VectorOf<int> getCoupledJoints()=0;
    virtual std::string getCoupledJointName(int joint)=0;
    virtual bool checkJointIsCoupled(int joint)=0;


    virtual void setCoupledJointLimit(int joint, const double& min, const double& max)=0;
    virtual void getCoupledJointLimit(int joint, double& min, double& max)=0;
};

#endif // YARP_DEV_IJOINTCOUPLING_H

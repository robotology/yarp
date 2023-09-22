/*
 * SPDX-FileCopyrightText: 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IJOINTCOUPLINGIMPL_H
#define YARP_DEV_IJOINTCOUPLINGIMPL_H

#include <yarp/dev/IJointCoupling.h>
#include <yarp/dev/api.h>

#include <vector>
#include <unordered_map>
#include <string>

namespace yarp::dev {
class ImplementJointCoupling;
}

class YARP_dev_API yarp::dev::ImplementJointCoupling: public IJointCoupling
{
public:
    /* Constructor.
     */
    ImplementJointCoupling() = default;

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementJointCoupling() = default;

    void initialise(yarp::sig::VectorOf<int> coupled_joints, std::vector<std::string> coupled_joint_names, std::vector<std::pair<double, double>> coupled_joint_limits);

    yarp::sig::VectorOf<int> getCoupledJoints() override final;
    std::string getCoupledJointName(int joint) override final;
    bool checkJointIsCoupled(int joint) override final;
    void setCoupledJointLimit(int joint, const double& min, const double& max) override final;
    void getCoupledJointLimit(int joint, double& min, double& max) override final;
protected:
    yarp::sig::VectorOf<int> m_coupledJoints;
    std::vector<std::string> m_coupledJointNames;
    std::unordered_map<int, std::pair<double, double>> m_coupledJointLimits;
    unsigned int m_controllerPeriod;
    unsigned int m_couplingSize;

};

#endif // YARP_DEV_IJOINTCOUPLINGIMPL_H
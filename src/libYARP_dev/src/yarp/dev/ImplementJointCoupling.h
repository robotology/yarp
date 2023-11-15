/*
 * SPDX-FileCopyrightText: 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IJOINTCOUPLINGIMPL_H
#define YARP_DEV_IJOINTCOUPLINGIMPL_H

#include <yarp/dev/IJointCoupling.h>
#include <yarp/dev/api.h>

#include <vector>
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

    void initialise(yarp::sig::VectorOf<size_t> coupled_physical_joints,
                    yarp::sig::VectorOf<size_t> coupled_actuated_axes,
                    std::vector<std::string> physical_joint_names,
                    std::vector<std::string> actuated_axes_names,
                    std::vector<std::pair<double, double>> coupled_physical_joint_limits);
    bool getNrOfPhysicalJoints(size_t& nrOfPhysicalJoints) override final;
    bool getNrOfActuatedAxes(size_t& nrOfActuatedAxes) override final;
    bool getCoupledPhysicalJoints(yarp::sig::VectorOf<size_t>& coupPhysJointsIndexes) override final;
    bool getCoupledActuatedAxes(yarp::sig::VectorOf<size_t>& coupActAxesIndexes) override final;
    bool getPhysicalJointName(size_t physicalJointIndex, std::string& physicalJointName) override final;
    bool getActuatedAxisName(size_t actuatedAxisIndex, std::string& actuatedAxisName) override final;
    bool getPhysicalJointLimits(size_t physicalJointIndex, double& min, double& max) override final;
protected:
    bool checkPhysicalJointIsCoupled(size_t physicalJointIndex);

    yarp::sig::VectorOf<size_t> m_coupledPhysicalJoints;
    yarp::sig::VectorOf<size_t> m_coupledActuatedAxes;
    std::vector<std::string> m_physicalJointNames;
    std::vector<std::string> m_actuatedAxesNames;
    std::vector<std::pair<double, double>> m_physicalJointLimits;
    unsigned int m_controllerPeriod;
    unsigned int m_couplingSize;

};

#endif // YARP_DEV_IJOINTCOUPLINGIMPL_H

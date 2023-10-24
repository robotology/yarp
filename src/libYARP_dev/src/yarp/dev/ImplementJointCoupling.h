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

    void initialise(yarp::sig::VectorOf<size_t> physical_joints,
                    yarp::sig::VectorOf<size_t> actuated_axes,
                    std::vector<std::string> physical_joint_names,
                    std::vector<std::string> actuated_axes_names,
                    std::vector<std::pair<double, double>> physical_joint_limits);

    yarp::sig::VectorOf<size_t> getCoupledPhysicalJoints() override final;
    yarp::sig::VectorOf<size_t> getCoupledActuatedAxes() override final;
    std::string getPhysicalJointName(size_t physicalJointIndex) override final;
    std::string getActuatedAxisName(size_t actuatedAxisIndex) override final;

    bool getPhysicalJointLimits(size_t physicalJointIndex, double& min, double& max) override final;
protected:
    bool checkPhysicalJointIsCoupled(size_t physicalJointIndex);

    yarp::sig::VectorOf<size_t> m_physicalJoints;
    yarp::sig::VectorOf<size_t> m_actuatedAxes;
    std::vector<std::string> m_physicalJointNames;
    std::vector<std::string> m_actuatedAxesNames;
    std::unordered_map<size_t, std::pair<double, double>> m_physicalJointLimits;
    unsigned int m_controllerPeriod;
    unsigned int m_couplingSize;

};

#endif // YARP_DEV_IJOINTCOUPLINGIMPL_H
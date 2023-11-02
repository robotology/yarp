/*
 * SPDX-FileCopyrightText: 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "ImplementJointCoupling.h"
#include <algorithm>


using namespace yarp::dev;


void ImplementJointCoupling::initialise(yarp::sig::VectorOf<size_t> coupled_physical_joints,
                                        yarp::sig::VectorOf<size_t> coupled_actuated_axes,
                                        std::vector<std::string> physical_joint_names,
                                        std::vector<std::string> actuated_axes_names,
                                        std::vector<std::pair<double, double>> physical_joint_limits)
{
    m_coupledPhysicalJoints     = coupled_physical_joints;
    m_coupledActuatedAxes       = coupled_actuated_axes;
    m_physicalJointNames        = physical_joint_names;
    m_actuatedAxesNames         = actuated_axes_names;
    // Configure a map between physical joints and limits
    for (std::size_t i = 0, j = 0; i < coupled_physical_joints.size(); i++)
    {
        const int physical_joint_index = coupled_physical_joints(i);
        std::string physical_joint_name {""};
        auto ok = getPhysicalJointName(physical_joint_index, physical_joint_name);
        if (physical_joint_name != "invalid" && physical_joint_name != "reserved")
        {
            m_physicalJointLimits[coupled_physical_joints[i]] = physical_joint_limits[j];
            j++;
        }
    }


}

bool ImplementJointCoupling::getNrOfPhysicalJoints(size_t* nrOfPhysicalJoints) {
    //TODO is it right?
    *nrOfPhysicalJoints = m_physicalJointLimits.size();
    return true;
}
bool ImplementJointCoupling::getNrOfActuatedAxes(size_t* nrOfActuatedAxes){
    // TODO is it right?
    *nrOfActuatedAxes = m_actuatedAxesNames.size();
    return true;
}

bool ImplementJointCoupling::getCoupledPhysicalJoints(yarp::sig::VectorOf<size_t>& coupPhysJointsIndexes) {
    coupPhysJointsIndexes = m_coupledPhysicalJoints;
    return true;
}

bool ImplementJointCoupling::getCoupledActuatedAxes(yarp::sig::VectorOf<size_t>& coupActAxesIndexes) {
    coupActAxesIndexes = m_coupledActuatedAxes;
    return true;
}

bool ImplementJointCoupling::getPhysicalJointName(size_t physicalJointIndex, std::string& physicalJointName){
    int c_joint = -1;
    // TODO refactor also here
    for (size_t i = 0; i < m_coupledPhysicalJoints.size(); ++i)
    {
        if (m_coupledPhysicalJoints[i]==physicalJointIndex) c_joint = i;
    }

    if (c_joint >= 0 && static_cast<size_t>(c_joint) < m_coupledPhysicalJoints.size())
    {
        physicalJointName = m_physicalJointNames[c_joint];
        return true;
    }
    else
    {
        physicalJointName = "invalid";
        return false;
    }
}

bool ImplementJointCoupling::getActuatedAxisName(size_t actuatedAxisIndex, std::string& actuatedAxisName){
    // TODO is it right?
    actuatedAxisName = m_actuatedAxesNames[actuatedAxisIndex];
    return true;
}

bool ImplementJointCoupling::checkPhysicalJointIsCoupled(size_t physicalJointIndex){
    return std::find(m_coupledPhysicalJoints.begin(), m_coupledPhysicalJoints.end(), physicalJointIndex) != m_coupledPhysicalJoints.end();
}

bool ImplementJointCoupling::getPhysicalJointLimits(size_t physicalJointIndex, double& min, double& max){
    std::string physical_joint_name{""};
    auto ok = getPhysicalJointName(physicalJointIndex, physical_joint_name);

    if (physical_joint_name != "reserved" && physical_joint_name != "invalid")
    {
        min = m_physicalJointLimits.at(physicalJointIndex).first;
        max = m_physicalJointLimits.at(physicalJointIndex).second;
        return true;
    }
    return false;
}

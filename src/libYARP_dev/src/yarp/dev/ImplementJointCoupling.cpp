/*
 * SPDX-FileCopyrightText: 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "ImplementJointCoupling.h"


using namespace yarp::dev;


void ImplementJointCoupling::initialise(yarp::sig::VectorOf<size_t> physical_joints, std::vector<std::string> physical_joint_names, std::vector<std::pair<double, double>> physical_joint_limits) {
    m_physicalJoints=physical_joints;
    m_physicalJointNames=physical_joint_names;

    // Configure a map between physical joints and limits
    for (std::size_t i = 0, j = 0; i < physical_joints.size(); i++)
    {
        const int physical_joint_index = physical_joints(i);
        const std::string physical_joint_name = getPhysicalJointName(physical_joint_index);
        if (physical_joint_name != "invalid" && physical_joint_name != "reserved")
        {
            m_physicalJointLimits[physical_joints[i]] = physical_joint_limits[j];
            j++;
        }
    }


}


yarp::sig::VectorOf<size_t> ImplementJointCoupling::getPhysicalJoints() {
    return m_physicalJoints;
}

std::string ImplementJointCoupling::getPhysicalJointName(size_t joint){
    int c_joint = -1;
    for (size_t i = 0; i < m_physicalJoints.size(); ++i)
    {
        if (m_physicalJoints[i]==joint) c_joint = i;
    }

    if (c_joint >= 0 && static_cast<size_t>(c_joint) < m_physicalJoints.size())
    {
        return m_physicalJointNames[c_joint];
    }
    else
    {
        return std::string("invalid");
    }
}

bool ImplementJointCoupling::checkPhysicalJointIsCoupled(size_t joint){
    for (size_t i = 0; i < m_physicalJoints.size(); ++i)
    {
        if (m_physicalJoints[i]==joint) return true;
    }
    return false;
}
bool ImplementJointCoupling::setPhysicalJointLimits(size_t joint, const double& min, const double& max){
    const std::string physical_joint_name = getPhysicalJointName(joint);

    if (physical_joint_name != "reserved" && physical_joint_name != "invalid")
    {
        m_physicalJointLimits.at(joint).first = min;
        m_physicalJointLimits.at(joint).second = max;
        return true;
    }
    return false;
}
bool ImplementJointCoupling::getPhysicalJointLimits(size_t joint, double& min, double& max){
    const std::string physical_joint_name = getPhysicalJointName(joint);

    if (physical_joint_name != "reserved" && physical_joint_name != "gyp_invalid")
    {
        min = m_physicalJointLimits.at(joint).first;
        max = m_physicalJointLimits.at(joint).second;
        return true;
    }
    return false;
}
/*
 * SPDX-FileCopyrightText: 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "ImplementJointCoupling.h"


using namespace yarp::dev;


void ImplementJointCoupling::initialise(yarp::sig::VectorOf<int> coupled_joints, std::vector<std::string> coupled_joint_names, std::vector<std::pair<double, double>> coupled_joint_limits) {
    m_coupledJoints=coupled_joints;
    m_coupledJointNames=coupled_joint_names;

    // Configure a map between coupled joints and limits
    for (std::size_t i = 0, j = 0; i < coupled_joints.size(); i++)
    {
        const int coupled_joint_index = coupled_joints(i);
        const std::string coupled_joint_name = getCoupledJointName(coupled_joint_index);
        if (coupled_joint_name != "invalid" && coupled_joint_name != "reserved")
        {
            m_coupledJointLimits[coupled_joints[i]] = coupled_joint_limits[j];
            j++;
        }
    }


}


yarp::sig::VectorOf<int> ImplementJointCoupling::getCoupledJoints() {
    return m_coupledJoints;
}

std::string ImplementJointCoupling::getCoupledJointName(int joint){
    int c_joint = -1;
    for (size_t i = 0; i < m_coupledJoints.size(); ++i)
    {
        if (m_coupledJoints[i]==joint) c_joint = i;
    }

    if (c_joint >= 0 && static_cast<size_t>(c_joint) < m_coupledJoints.size())
    {
        return m_coupledJointNames[c_joint];
    }
    else
    {
        return std::string("invalid");
    }
}

bool ImplementJointCoupling::checkJointIsCoupled(int joint){
    for (size_t i = 0; i < m_coupledJoints.size(); ++i)
    {
        if (m_coupledJoints[i]==joint) return true;
    }
    return false;
}
void ImplementJointCoupling::setCoupledJointLimit(int joint, const double& min, const double& max){
    const std::string coupled_joint_name = getCoupledJointName(joint);

    if (coupled_joint_name != "reserved" && coupled_joint_name != "invalid")
    {
        m_coupledJointLimits.at(joint).first = min;
        m_coupledJointLimits.at(joint).second = max;
    }
}
void ImplementJointCoupling::getCoupledJointLimit(int joint, double& min, double& max){
    const std::string coupled_joint_name = getCoupledJointName(joint);

    if (coupled_joint_name != "reserved" && coupled_joint_name != "gyp_invalid")
    {
        min = m_coupledJointLimits.at(joint).first;
        max = m_coupledJointLimits.at(joint).second;
    }
}
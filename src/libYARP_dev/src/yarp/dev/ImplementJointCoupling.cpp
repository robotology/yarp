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
    m_physicalJointLimits       = physical_joint_limits;
}

bool ImplementJointCoupling::getNrOfPhysicalJoints(size_t& nrOfPhysicalJoints) {
    nrOfPhysicalJoints = m_physicalJointNames.size();
    return true;
}
bool ImplementJointCoupling::getNrOfActuatedAxes(size_t& nrOfActuatedAxes){
    nrOfActuatedAxes = m_actuatedAxesNames.size();
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
    if(physicalJointIndex >= m_physicalJointNames.size()) {
        return false;
    }
    else {
        physicalJointName=m_physicalJointNames[physicalJointIndex];
        return true;
    }

}

bool ImplementJointCoupling::getActuatedAxisName(size_t actuatedAxisIndex, std::string& actuatedAxisName){
    if(actuatedAxisIndex >= m_actuatedAxesNames.size()) {
        return false;
    }
    else {
        actuatedAxisName=m_actuatedAxesNames[actuatedAxisIndex];
        return true;
    }
}

bool ImplementJointCoupling::checkPhysicalJointIsCoupled(size_t physicalJointIndex){
    return std::find(m_coupledPhysicalJoints.begin(), m_coupledPhysicalJoints.end(), physicalJointIndex) != m_coupledPhysicalJoints.end();
}

bool ImplementJointCoupling::getPhysicalJointLimits(size_t physicalJointIndex, double& min, double& max){
    size_t nrOfPhysicalJoints;
    auto ok = getNrOfPhysicalJoints(nrOfPhysicalJoints);
    if (ok && physicalJointIndex < nrOfPhysicalJoints)
    {
        min = m_physicalJointLimits[physicalJointIndex].first;
        max = m_physicalJointLimits[physicalJointIndex].second;
        return true;
    }
    return false;
}

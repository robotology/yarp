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

ReturnValue ImplementJointCoupling::getNrOfPhysicalJoints(size_t& nrOfPhysicalJoints) {
    nrOfPhysicalJoints = m_physicalJointNames.size();
    return ReturnValue_ok;
}
ReturnValue ImplementJointCoupling::getNrOfActuatedAxes(size_t& nrOfActuatedAxes){
    nrOfActuatedAxes = m_actuatedAxesNames.size();
    return ReturnValue_ok;
}

ReturnValue ImplementJointCoupling::getCoupledPhysicalJoints(yarp::sig::VectorOf<size_t>& coupPhysJointsIndexes) {
    coupPhysJointsIndexes = m_coupledPhysicalJoints;
    return ReturnValue_ok;
}

ReturnValue ImplementJointCoupling::getCoupledActuatedAxes(yarp::sig::VectorOf<size_t>& coupActAxesIndexes) {
    coupActAxesIndexes = m_coupledActuatedAxes;
    return ReturnValue_ok;
}

ReturnValue ImplementJointCoupling::getPhysicalJointName(size_t physicalJointIndex, std::string& physicalJointName){
    if(physicalJointIndex >= m_physicalJointNames.size()) {
        return ReturnValue::return_code::return_value_error_input_out_of_bounds;
    }
    else {
        physicalJointName=m_physicalJointNames[physicalJointIndex];
        return ReturnValue_ok;
    }

}

ReturnValue ImplementJointCoupling::getActuatedAxisName(size_t actuatedAxisIndex, std::string& actuatedAxisName){
    if(actuatedAxisIndex >= m_actuatedAxesNames.size()) {
        return ReturnValue::return_code::return_value_error_input_out_of_bounds;
    }
    else {
        actuatedAxisName=m_actuatedAxesNames[actuatedAxisIndex];
        return ReturnValue_ok;
    }
}

bool ImplementJointCoupling::checkPhysicalJointIsCoupled(size_t physicalJointIndex){
    return std::find(m_coupledPhysicalJoints.begin(), m_coupledPhysicalJoints.end(), physicalJointIndex) != m_coupledPhysicalJoints.end();
}

ReturnValue ImplementJointCoupling::getPhysicalJointLimits(size_t physicalJointIndex, double& min, double& max){
    size_t nrOfPhysicalJoints;
    auto ok = getNrOfPhysicalJoints(nrOfPhysicalJoints);
    if (ok && physicalJointIndex < nrOfPhysicalJoints)
    {
        min = m_physicalJointLimits[physicalJointIndex].first;
        max = m_physicalJointLimits[physicalJointIndex].second;
        return ReturnValue_ok;
    }
    return ReturnValue::return_code::return_value_error_input_out_of_bounds;
}

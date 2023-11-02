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
 * Interface for joint coupling. It contains the methods to convert from Physical Joints to Actuated Axes and viceversa.
 *
 * $$
 * \theta \in \mathbb{R}^{m} : \text{Actuated Axes positions}
 * $$
 *
 * $$
 * q \in \mathbb{R}^{n} : \text{Physical Joints positions}
 * $$
 *
 * In general, we have $m \leq n$.
 *
 * There is unique value of Actuated Axes corresponding to a given Physical Joints position, while in general there may be multiple Physical Joints position corresponding to a given Actuated Axes.
 *
 * So, the mapping from Physical Joints $q$ to Actuated Axes $\theta$ is defined as:
 *
 * $$
 * \theta = f(q)
 * $$
 *
 * while the mapping from Actuated Axes $\theta$  to Physical Joints $q$  is defined as:
 *
 * $$
 * q = g(\theta)
 * $$
 *
 * We have the following property:
 *
 * $$
 * f(g(\theta)) = \theta
 * $$
 *
 * while in general it is not always true that the inverse holds, i.e.
 *
 * $$
 * g(f(q)) \neq q
 * $$
 *
 * In the rest of the section, we assume that $\theta(t) = f(q(t))$ and $\dot{\theta}(t)^T \tau_{\theta}(t) = \dot{q}(t)^T \tau_{q}(t)$
 */
class YARP_dev_API yarp::dev::IJointCoupling
{
public:
    /**
     * Destructor.
     */
    virtual ~IJointCoupling() {}

    /**
     * @brief Convert from Physical Joints to Actuated Axes position.
     * This method implements $f(q)$.
     *
     * @param[in] physJointsPos Physical Joints position
     * @param[out] actAxesPos Actuated Axes position
     * @return, true/false on success/failure
     */
    virtual bool convertFromPhysicalJointsToActuatedAxesPos(const yarp::sig::Vector& physJointsPos, yarp::sig::Vector& actAxesPos) = 0;

    /**
     * @brief Convert from Physical Joints to Actuated Axes velocity.
     * This method implements $f'$ that can be used to compute $\dot{\theta}(t)$, defined such that:
     *
     * $$
     * \dot{\theta}(t) = \frac{\partial}{\partial q} f(q(t)) \dot{q}(t) = f'( q(t), \dot{q}(t))
     * $$
     *
     * @param[in] physJointsPos Physical Joints position
     * @param[in] physJointsVel Physical Joints velocity
     * @param[out] actAxesVel  Actuated Axes velocity
     * @return, true/false on success/failure
     */
    virtual bool convertFromPhysicalJointsToActuatedAxesVel(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsVel, yarp::sig::Vector& actAxesVel) = 0;

    /**
     * @brief Convert from Physical Joints to Actuated Axes acceleration.
     * This method implements $f''$ that can be used to compute $\ddot{\theta}(t)$,  defined such that:
     *
     * $$
     * \ddot{\theta}(t) = \frac{\partial}{\partial q} f'(q(t), \dot{q}(t)) \dot{q}(t) + \frac{\partial}{\partial \dot{q}} f'(q(t), \dot{q}(t)) \ddot{q}(t) = f''( q(t), \dot{q}(t), \ddot{q}(t))
     * $$
     *
     * @param[in] physJointsPos Physical Joints position
     * @param[in] physJointsVel Physical Joints velocity
     * @param[in] physJointsAcc Physical Joints acceleration
     * @param[out] actAxesAcc  Actuated Axes acceleration
     * @return, true/false on success/failure
     */
    virtual bool convertFromPhysicalJointsToActuatedAxesAcc(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsVel, const yarp::sig::Vector& physJointsAcc, yarp::sig::Vector& actAxesAcc) = 0;

    /**
     * @brief Convert from Physical Joints to Actuated Axes torque
     *
     * @param[in] physJointsPos Physical Joints position
     * @param[in] physJointsTrq Physical Joints torque
     * @param[out] actAxesTrq  Actuated Axes torque
     * @return true/false on success/failure
     */
    virtual bool convertFromPhysicalJointsToActuatedAxesTrq(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsTrq, yarp::sig::Vector& actAxesTrq) = 0;

    /**
     * @brief Convert from Actuated Axes to Physical Joints position
     * This method implements $g(\theta)$.
     * @param[in] actAxesPos Actuated Axes position
     * @param[out] physJointsPos Physical Joints position
     * @return true/false on success/failure
     */
    virtual bool convertFromActuatedAxesToPhysicalJointsPos(const yarp::sig::Vector& actAxesPos, yarp::sig::Vector& physJointsPos) = 0;

    /**
     * @brief Convert from Actuated Axes to Physical Joints velocity
     * This method implements $g'$ that can be used to compute $\dot{q}(t)$, defined such that:
     *
     * $$
     * \dot{q}(t) = \frac{\partial}{\partial \theta} g(\theta(t)) \dot{\theta}(t) = g'( \theta(t), \dot{\theta}(t))
     * $$
     *
     * @param[in] actAxesPos Actuated Axes position
     * @param[in] actAxesVel Actuated Axes velocity
     * @param[out] physJointsVel Physical Joints velocity
     * @return true/false on success/failure
     */
    virtual bool convertFromActuatedAxesToPhysicalJointsVel(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesVel, yarp::sig::Vector& physJointsVel) = 0;

    /**
     * @brief Convert from Actuated Axes to Physical Joints acceleration
     *
     * This method implements $f''$ that can be used to compute $\ddot{\theta}(t)$,  defined such that:
     *
     * $$
     * \ddot{q}(t) = \frac{\partial}{\partial \theta} g'(\theta(t), \dot{\theta}(t)) \dot{\theta}(t) + \frac{\partial}{\partial \dot{\theta}} g'(\theta(t), \dot{\theta}(t)) \ddot{\theta}(t) = g''( \theta(t), \dot{\theta}(t), \ddot{\theta}(t))
     * $$
     *
     * @param[in] actAxesPos Actuated Axes position
     * @param[in] actAxesVel Actuated Axes velocity
     * @param[in] actAxesAcc Actuated Axes acceleration
     * @param[out] physJointsAcc Physical Joints acceleration
     * @return true/false on success/failure
     */
    virtual bool convertFromActuatedAxesToPhysicalJointsAcc(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesVel, const yarp::sig::Vector& actAxesAcc, yarp::sig::Vector& physJointsAcc) = 0;

    /**
     * @brief Convert from Actuated Axes to Physical Joints acceleration
     *
     * @param[in] actAxesPos Actuated Axes position
     * @param[in] actAxesTrq Actuated Axes torque
     * @param[out] physJointsTrq Physical Joints torque
     * @return true/false on success/failure
     */
    virtual bool convertFromActuatedAxesToPhysicalJointsTrq(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesTrq, yarp::sig::Vector& physJointsTrq) = 0;

    // In some case, for a given coupling several "physical joints" ad "actuated axis"
    // may be related in a obvious way, i.e. the position and torque of given physical
    // joint could be equal to the position and torque of given actuated axis.
    // In that case "physical joints" ad "actuated axis" are typically identified by the
    // same name. The getCoupled***() methods return the indices of the "actuated axis"
    // and "physical joints" that are coupled in a non-obvious way

    /**
     * @brief Get the number of physical joints
     *
     * @param[out] nrOfPhysicalJoints The number of physical joints
     * @return true/false on success/failure
     */
    virtual bool getNrOfPhysicalJoints(size_t* nrOfPhysicalJoints) = 0;

    /**
     * @brief Get the number of actuated axes
     *
     * @param nrOfActuatedAxes The number of actuated axes
     * @return true/false on success/failure
     */
    virtual bool getNrOfActuatedAxes(size_t* nrOfActuatedAxes) = 0;

    /**
     * @brief Return the vector of "physical joints indices" (i.e. numbers from 0 to n-1)
     * that are related to actuated axis in a non-obvious way
     * @param[out] coupPhysJointsIndexes the vector of "physical joints indices"
     * @return true/false on success/failure
     */
    virtual bool getCoupledPhysicalJoints(yarp::sig::VectorOf<size_t>& coupPhysJointsIndexes)=0;

    /**
     * @brief Return the vector of "actuator axis indices" (i.e. numbers from 0 to m-1)
     * that are related to physical joints in a non-obvious way
     *
     * @param[out] coupActAxesIndexes the vector of "actuator axis indices"
     * @return true/false on success/failure
     */
    virtual bool getCoupledActuatedAxes(yarp::sig::VectorOf<size_t>& coupActAxesIndexes)=0;

    /**
     * @brief Get the name of an actuated axis
     *
     * @param[in] actuatedAxisIndex  the number from 0 to m-1 that identifies
     * the location of a "actuated axis" in a actuated axis vector.
     * @param[out] actuatedAxisName the actuated axis name
     * @return true/false on success/failure
     */
    virtual bool getActuatedAxisName(size_t actuatedAxisIndex, std::string& actuatedAxisName)=0;

    /**
     * @brief Get the name of a physical joint
     *
     * @param[in] physicalJointIndex the number from 0 to n-1 that identifies
     * the location of a "physical joint" in a physical joint vector
     * @param[out] physicalJointName the physical joint name
     * @return true/false on success/failure
     */
    virtual bool getPhysicalJointName(size_t physicalJointIndex, std::string& physicalJointName)=0;

    /**
     * @brief Get the Physical Joint Limit object
     *
     * @param[in] physicalJointIndex the number from 0 to n-1 that identifies
     * the location of a "physical joint" in a physical joint vector
     * @param[out] min minimum value
     * @param[out] max maximum value
     * @return true/false on success/failure
     */
    virtual bool getPhysicalJointLimits(size_t physicalJointIndex, double& min, double& max)=0;
};

#endif // YARP_DEV_IJOINTCOUPLING_H

/*
 * SPDX-FileCopyrightText: 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IJACOBIANCOUPLING_H
#define YARP_DEV_IJACOBIANCOUPLING_H

#include <yarp/dev/api.h>
#include <yarp/os/Vocab.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>

namespace yarp::dev {
class IJacobianCoupling;
}
/**
 * @ingroup dev_iface_motor
 *
 * Interface for jacobian coupling. It contains the methods to get the Jacobian mapping the Physical Joints to Actuated Axes and viceversa.
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
class YARP_dev_API yarp::dev::IJacobianCoupling
{
public:
    /**
     * Destructor.
     */
    virtual ~IJacobianCoupling() {}

    /**
     * @brief Get the Jacobian mapping the Actuated Axes to Physical Joints velocity
     *
     * This method implements $\frac{\partial}{\partial \theta} g(\theta(t))$, defined such that:
     *
     * $$
     * \dot{q}(t) = \frac{\partial}{\partial \theta} g(\theta(t)) \dot{\theta}(t)
     * $$
     *
     * @return true/false on success/failure
     * @param[in] actAxesPos Actuated Axes position
     * @param[out] actAxesToPhysJointsVelJacobian Jacobian mapping the Actuated
     * Axes to Physical Joints velocity
     */
    virtual bool evaluateJacobianFromActuatedAxesToPhysicalJointsVel(const yarp::sig::Vector& actAxesPos, yarp::sig::Matrix& actAxesToPhysJointsVelJacobian)=0;

    /**
     * @brief Get the Jacobian mapping the Physical Joints to Actuated Axes velocity
     *
     * This method implements $\frac{\partial}{\partial \q} f(\q(t))$, defined such that:
     *
     * $$
     * \dot{theta}(t) = \frac{\partial}{\partial \q} f(\q(t)) \dot{\q}(t)
     * $$
     *
     * @return true/false on success/failure
     * @param[in] physJointsPos Physical Joints position
     * @param[out] physJointsToActAxesVelJacobian Jacobian mapping the Physical Joints
     * to Actuated Axes velocity
     */
    virtual bool evaluateJacobianFromPhysicalJointsToActuatedAxeseVel(const yarp::sig::Vector& physJointsPos, yarp::sig::Matrix& physJointsToActAxesVelJacobian)=0;
};

#endif // YARP_DEV_IJACOBIANCOUPLING_H

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
 * Interface for joint coupling. It contains the methods to convert from Physical Joint to Actuated Axes and viceversa.
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
 * There is unique value of Actuated Axes corresponding to a given Physical Joint position, while in general there may be multiple Physical Joint position corresponding to a given Actuated Axes.
 *
 * So, the mapping from Physical Joint $q$ to Actuated Axes $\theta$ is defined as:
 *
 * $$
 * \theta = f(q)
 * $$
 *
 * while the mapping from Actuated Axes $\theta$  to Physical Joint $q$  is defined as:
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
     * @brief Convert from Physical Joint to Actuated Axes position.
     * This method implements $f(q)$.
     *
     * @param[in] physJointPos Physical Joint position
     * @param[out] actAxisPos Actuated Axes position
     * @return, true/false on success/failure
     */
    virtual bool convertFromPhysicalJointPosToActuatedAxisPos(const yarp::sig::Vector& physJointPos, yarp::sig::Vector& actAxisPos) = 0;

    /**
     * @brief Convert from Physical Joint to Actuated Axes velocity.
     * This method implements $f'$ that can be used to compute $\dot{\theta}(t)$, defined such that:
     *
     * $$
     * \dot{\theta}(t) = \frac{\partial}{\partial q} f(q(t)) \dot{q}(t) = f'( q(t), \dot{q}(t))
     * $$
     *
     * @param[in] physJointPos Physical Joint position
     * @param[in] physJointVel Physical Joint velocity
     * @param[out] actAxisVel  Actuated Axes velocity
     * @return, true/false on success/failure
     */
    virtual bool convertFromPhysicalJointVelToActuatedAxisVel(const yarp::sig::Vector& physJointPos, const yarp::sig::Vector& physJointVel, yarp::sig::Vector& actAxisVel) = 0;

    /**
     * @brief Convert from Physical Joint to Actuated Axes acceleration.
     * This method implements $f''$ that can be used to compute $\ddot{\theta}(t)$,  defined such that:
     *
     * $$
     * \ddot{\theta}(t) = \frac{\partial}{\partial q} f'(q(t), \dot{q}(t)) \dot{q}(t) + \frac{\partial}{\partial \dot{q}} f'(q(t), \dot{q}(t)) \ddot{q}(t) = f''( q(t), \dot{q}(t), \ddot{q}(t))
     * $$
     *
     * @param[in] physJointPos Physical Joint position
     * @param[in] physJointVel Physical Joint velocity
     * @param[in] physJointAcc Physical Joint acceleration
     * @param[out] actAxisAcc  Actuated Axes acceleration
     * @return, true/false on success/failure
     */
    virtual bool convertFromPhysicalJointPosToActuatedAxisAcc(const yarp::sig::Vector& physJointPos, const yarp::sig::Vector& physJointVel, const yarp::sig::Vector& physJointAcc, yarp::sig::Vector& actAxisAcc) = 0;

    /**
     * @brief Convert from Physical Joint to Actuated Axes torque
     *
     * @param[in] physJointPos Physical Joint position
     * @param[in] physJointTrq Physical Joint torque
     * @param[out] actAxisTrq  Actuated Axes torque
     * @return true/false on success/failure
     */
    virtual bool convertFromPhysicalJointTrqToActuatedAxisTrq(const yarp::sig::Vector& physJointPos, const yarp::sig::Vector& physJointTrq, yarp::sig::Vector& actAxisTrq) = 0;

    /**
     * @brief Convert from Actuated Axes to Physical Joint position
     * This method implements $g(\theta)$.
     * @param[in] actAxisPos Actuated Axes position
     * @param[out] physJointPos Physical Joint position
     * @return true/false on success/failure
     */
    virtual bool convertFromActuatedAxisToPhysicalJointPos(const yarp::sig::Vector& actAxisPos, yarp::sig::Vector& physJointPos) = 0;

    /**
     * @brief Convert from Actuated Axes to Physical Joint velocity
     * This method implements $g'$ that can be used to compute $\dot{q}(t)$, defined such that:
     *
     * $$
     * \dot{q}(t) = \frac{\partial}{\partial \theta} g(\theta(t)) \dot{\theta}(t) = g'( \theta(t), \dot{\theta}(t))
     * $$
     *
     * @param[in] actAxisPos Actuated Axes position
     * @param[in] actAxisVel Actuated Axes velocity
     * @param[out] physJointVel Physical Joint velocity
     * @return true/false on success/failure
     */
    virtual bool convertFromActuatedAxisToPhysicalJointVel(const yarp::sig::Vector& actAxisPos, const yarp::sig::Vector& actAxisVel, yarp::sig::Vector& physJointVel) = 0;

    /**
     * @brief Convert from Actuated Axes to Physical Joint acceleration
     *
     * This method implements $f''$ that can be used to compute $\ddot{\theta}(t)$,  defined such that:
     *
     * $$
     * \ddot{q}(t) = \frac{\partial}{\partial \theta} g'(\theta(t), \dot{\theta}(t)) \dot{\theta}(t) + \frac{\partial}{\partial \dot{\theta}} g'(\theta(t), \dot{\theta}(t)) \ddot{\theta}(t) = g''( \theta(t), \dot{\theta}(t), \ddot{\theta}(t))
     * $$
     *
     * @param[in] actAxisPos Actuated Axes position
     * @param[in] actAxisVel Actuated Axes velocity
     * @param[in] actAxisAcc Actuated Axes acceleration
     * @param[out] physJointAcc Physical Joint acceleration
     * @return true/false on success/failure
     */
    virtual bool convertFromActuatedAxisToPhysicalJointAcc(const yarp::sig::Vector& actAxisPos, const yarp::sig::Vector& actAxisVel, const yarp::sig::Vector& actAxisAcc, yarp::sig::Vector& physJointAcc) = 0;

    /**
     * @brief Convert from Actuated Axes to Physical Joint acceleration
     *
     * @param[in] actAxisPos Actuated Axes position
     * @param[in] actAxisTrq Actuated Axes torque
     * @param[out] physJointTrq Physical Joint torque
     * @return true/false on success/failure
     */
    virtual bool convertFromActuatedAxisToPhysicalJointTrq(const yarp::sig::Vector& actAxisPos, const yarp::sig::Vector& actAxisTrq, yarp::sig::Vector& physJointTrq) = 0;

    /**
     * @brief Get the physical joints object
     *
     * @return yarp::sig::VectorOf<int>
     */
    virtual yarp::sig::VectorOf<size_t> getPhysicalJoints()=0;

    /**
     * @brief Get the name of a physical joint
     *
     * @param joint index of the joint
     * @return name of the joint
     */
    virtual std::string getPhysicalJointName(size_t joint)=0;

    /**
     * @brief Check if a physical joint is coupled
     *
     * @param joint index of the joint
     * @return true/false on coupled/not coupled
     */
    virtual bool checkPhysicalJointIsCoupled(size_t joint)=0;

    /**
     * @brief Set limts for a physical joint
     *
     * @param[in] joint index of the joint
     * @param[in] min minimum value
     * @param[in] max maximum value
     * @return true/false on success/failure
     */
    virtual bool setPhysicalJointLimits(size_t joint, const double& min, const double& max)=0;

    /**
     * @brief Get the Physical Joint Limit object
     *
     * @param[in] joint index of the joint
     * @param[out] min minimum value
     * @param[out] max maximum value
     * @return true/false on success/failure
     */
    virtual bool getPhysicalJointLimits(size_t joint, double& min, double& max)=0;
};

#endif // YARP_DEV_IJOINTCOUPLING_H

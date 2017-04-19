/*
 * Copyright (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Valentina Gaggero <valentina.gaggero@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_RTF_JOINTSPOSMOVE_H
#define YARP_RTF_JOINTSPOSMOVE_H

#include <yarp/rtf/api.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/PolyDriver.h>
#include <string>

namespace yarp {
namespace rtf {

/**
 * \brief The JointPosMotion class is used
 * has been created to simplify and speed up development of tests
 * that controls joints in position.
 */
class YARP_rtf_API jointsPosMotion {
public:
    //jointsPosMotion();

    /**
     * @brief Creates an object for control joints specified in @jlist.
     *
     * Sets default speed used in position control.
     *
     * Tolerance and timeout will be initialized with default values
     * (tolerance=1.0 deg, timeout=5 sec)
     *
     * @param polydriver: pointer to polydriver used to manage joints
     * @param jlist: list of joints
     */
    jointsPosMotion(yarp::dev::PolyDriver *polydriver,
                    yarp::sig::Vector &jlist);

    virtual ~jointsPosMotion();

    /**
     * @brief Sets tolerance used to check if a joint is in position.
     *
     * @param tolerance
     */
    void setTolerance(double tolerance);

    /**
     * @brief Sets timeout.
     *
     * @param timeout in seconds
     */
    void setTimeout(double timeout);

    /**
     * @brief Sets speed of each joint used in position control.
     *
     * @param vector of speed. The vector must have size equal to @jlist in
     *               costructor.
     */
    void setSpeed(yarp::sig::Vector &speedlist);

    /**
     * @brief Sets all joints in position control mode and checks if all are in
     * position control mode waiting timeout seconds.
     *
     * The value of timeout can be default or configured by setTimeout function.
     *
     * @return: true if all joints are in position, false otherwise
     */
    bool setAndCheckPosControlMode();


    /**
     * @brief Moves joint @a j in position @a pos and checks if joint reaches
     * target within tolerance range in maximun timeout seconds.
     *
     * @param j: joint
     * @param pos: target position
     * @param reached_pos: if not null, in output will contain the reached
     *                     position
     * @return: true if the joint has reached target position, false otherwise.
     */
    bool goToSingle(int j,
                    double pos,
                    double *reached_pos = YARP_NULLPTR);

    /**
     * @brief Moves joints in corrisponding positions specified by
     * @a positions and checks if all joints reach its target within tolerance
     * range in maximun timeout seconds.
     *
     * @param positions: vector of target positions
     * @param reached_pos: if not null, in output will contain the reached
     *                     position of each joint
     * @return: true if each joint has reached its target, false otherwise.
     */
    bool goTo(yarp::sig::Vector positions,
              yarp::sig::Vector *reached_pos = YARP_NULLPTR);

    /**
     * @brief Checks if joint @a j has reached its limit within tollerance
     *        range.
     *
     * @param j: joint to check.
     * @return: true if joint is on limit, false otherwise.
     */
    bool checkJointLimitsReached(int j);

private:
    class Private;
    Private * const mPriv;
};

} // namespace rtf
} // namespace yarp

#endif // YARP_RTF_JOINTSPOSMOVE_H

// -*- mode:C++ { } tab-width:4 { } c-basic-offset:4 { } indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Valentina Gaggero
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _JOINTSPOSMOVE_H_
#define _JOINTSPOSMOVE_H_

#include <string>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>

/**
 * \brief The JointPosMotion class is used
 * has been created to simplify and speed up development of tests
 * that controls joints in position.
 */
namespace RTF {
    namespace YARP {

class jointsPosMotion{
public:
    //jointsPosMotion();

    /**
     * Creates an object for control joints specified in @jlist.
     * Sets default speed used in position control.
     * Tolerance and timeout will be initialized with default values
     * (tolerance=1.0 deg  timeout=5 sec)
     * @param polydriver: pointer to polydriver used to manage joints
     * @param jlist: list of joints
     */
    jointsPosMotion(yarp::dev::PolyDriver *polydriver, yarp::sig::Vector &jlist);

    virtual ~jointsPosMotion();

    /**
     * Sets tolerance used to check if a joint is in position
     * @param tolerance
     * @return: -
     */
    void setTolerance(double tolerance);

    /**
     * Sets timeout
     * @param timeout in seconds
     * @return: -
     */
    void setTimeout(double timeout);

    /**
     * Sets speed of each joint used in position control
     * @param vector of speed. The vector must have size equal to @jlist in costructor.
     * @return: -
     */
    void setSpeed(yarp::sig::Vector &speedlist);

    /**
     * Sets all joints in position control mode and checks if all are in position control mode
     * waiting timeout seconds. (The value of timeout can be default or configured by setTimeout function)
     * @return: true if all joints are in position, false otherwise
     */
    bool setAndCheckPosControlMode();


    /**
     * Moves joint @j in position @pos and checks if joint reaches target within tolerance range
     * in maximun timeout seconds.
     * @param j: joint
     * @param pos: target position
     * @param reached_pos: if not null, in output will contain the reached position
     * @return: true if the joint has reached target position, false otherwise.
     */
    bool goToSingle(int j, double pos, double *reached_pos=NULL);

    /**
     * Moves joints in corrisponding positions specified by @positions and
     * checks if all joints reach its target within tolerance range in maximun timeout seconds.
     * @param positions: vector of target positions
     * @param reached_pos: if not null, in output will contain the reached position of each joint
     * @return: true if each joint has reached its target, false otherwise.
     */
    bool goTo(yarp::sig::Vector positions, yarp::sig::Vector *reached_pos=NULL);

    /**
     * Checks if joint @j has reached its limit withi tollerance range.
     * @return: true if joint is on limit, false otherwise.
     */
    bool checkJointLimitsReached(int j);



private:
    int getIndexOfJoint(int j);
    void readJointsLimits(void);
    void init(yarp::dev::PolyDriver *polydriver);
    yarp::sig::Vector jointsList;
    yarp::sig::Vector encoders;
    yarp::sig::Vector speed;
    yarp::sig::Vector max_lims;
    yarp::sig::Vector min_lims;
    double tolerance;
    double timeout;
    int    n_joints;

    yarp::dev::PolyDriver        *dd;
    yarp::dev::IPositionControl2 *ipos;
    yarp::dev::IControlMode2     *icmd;
    yarp::dev::IInteractionMode  *iimd;
    yarp::dev::IEncoders         *ienc;
    yarp::dev::IControlLimits    *ilim;

};
}
}
#endif //_JOINTSPOSMOVE_H_

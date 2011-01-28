// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2008 RobotCub Consortium
* Authors: Lorenzo Natale and Ugo Pattacini
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#ifndef __YARPCARTESIANCONTROLINTERFACES__
#define __YARPCARTESIANCONTROLINTERFACES__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Vector.h>

/*! \file CartesianController.h define control board standard interfaces */

namespace yarp {
    namespace dev {
        class ICartesianControl;
    }
}


/**
 * @ingroup dev_iface_motor
 *
 * Interface for a cartesian controller.
 */
class YARP_dev_API yarp::dev::ICartesianControl
{
public:
    /**
     * Destructor.
     */
    virtual ~ICartesianControl() {}

    /**
    * Set the controller in tracking or non-tracking mode. [wait for
    * reply] 
    * @param f true for tracking mode, false otherwise. 
    * @return true/false on success/failure. 
    *  
    * @note In tracking mode when the controller reachs the target, 
    *       it keeps on running in order to mantain the limb in the
    *       desired pose. In non-tracking mode the controller
    *       releases the limb as soon as the desired pose is
    *       reached.
    */
    virtual bool setTrackingMode(const bool f)=0;

    /**
    * Get the current controller mode. [wait for reply]
    * @param f here is returned true if controller is in tracking 
    *         mode, false otherwise.
    * @return true/false on success/failure. 
    */
    virtual bool getTrackingMode(bool *f)=0;

    /**
    * Get the current pose of the end-effector. [do not wait for 
    * reply] 
    * @param x a 3-d vector which is filled with the actual 
    *         position x,y,z (meters).
    * @param od a 4-d vector which is filled with the actual 
    * orientation using axis-angle representation xa, ya, za, theta 
    * (meters and radians). 
    * @return true/false on success/failure.
    */
    virtual bool getPose(yarp::sig::Vector &x, yarp::sig::Vector &o)=0;

    /**
    * Get the current pose of the specified link belonging to the 
    * kinematic chain. [wait for reply] 
    * @param axis joint index (regardless if it is actuated or 
    *            not). 
    * @param x a 3-d vector which is filled with the actual position
    *         x,y,z (meters) of the given link reference frame.
    * @param od a 4-d vector which is filled with the actual 
    * orientation of the given link reference frame using axis-angle
    * representation xa, ya, za, theta (meters and radians). 
    * @return true/false on success/failure.
    */
    virtual bool getPose(const int axis, yarp::sig::Vector &x, yarp::sig::Vector &o)=0;

    /**
    * Move the end-effector to a specified pose (position
    * and orientation) in cartesian space. [do not wait for reply]
    * @param xd a 3-d vector which contains the desired position 
    *           x,y,z
    * @param od a 4-d vector which contains the desired orientation
    * using axis-angle representation (xa, ya, za, theta). 
    * @param t set the trajectory duration time (seconds). If t<=0 
    *         (as by default) the current execution time is kept.
    * @return true/false on success/failure. 
    *  
    * @note Intended for streaming mode. 
    */
    virtual bool goToPose(const yarp::sig::Vector &xd, const yarp::sig::Vector &od,
                          const double t=0.0)=0;

    /**
    * Move the end-effector to a specified position in cartesian 
    * space, ignore the orientation. [do not wait for reply] 
    * @param xd a 3-d vector which contains the desired position 
    *          x,y,z (meters).
    * @param t set the trajectory duration time (seconds). If t<=0 
    *         (as by default) the current execution time is kept. 
    * @return true/false on success/failure. 
    *  
    * @note Intended for streaming mode. 
    */
    virtual bool goToPosition(const yarp::sig::Vector &xd, const double t=0.0)=0;

    /**
    * Move the end-effector to a specified pose (position
    * and orientation) in cartesian space. [wait for reply]
    * @param xd a 3-d vector which contains the desired position 
    *          x,y,z (meters).
    * @param od a 4-d vector which contains the desired orientation
    * using axis-angle representation (xa, ya, za, theta). 
    * @param t set the trajectory duration time (seconds). If t<=0 
    *         (as by default) the current execution time is kept.
    * @return true/false on success/failure.
    */
    virtual bool goToPoseSync(const yarp::sig::Vector &xd, const yarp::sig::Vector &od,
                              const double t=0.0)=0;

    /**
    * Move the end-effector to a specified position in cartesian 
    * space, ignore the orientation. [wait for reply] 
    * @param xd a 3-d vector which contains the desired position 
    *          x,y,z (meters).
    * @param t set the trajectory duration time (seconds). If t<=0 
    *         (as by default) the current execution time is kept. 
    * @return true/false on success/failure.
    */
    virtual bool goToPositionSync(const yarp::sig::Vector &xd, const double t=0.0)=0;

    /**
    * Get the actual desired pose and joints configuration as result
    * of kinematic inversion. [wait for reply] 
    * @param xdhat a 3-d vector which is filled with the actual 
    *          desired position x,y,z (meters); it may differ from
    *          the commanded xd.
    * @param odhat a 4-d vector which is filled with the actual 
    *          desired orientation using axis-angle representation
    *          xa, ya, za, theta (meters and radians); it may differ
    *          from the commanded od. 
    * @param qdhat the joints configuration through which the
    *             couple (xdhat,odhat) is achieved (degrees).
    * @return true/false on success/failure.
    */
    virtual bool getDesired(yarp::sig::Vector &xdhat, yarp::sig::Vector &odhat,
                            yarp::sig::Vector &qdhat)=0;

    /**
    * Ask for inverting a given pose without actually moving there.
    * [wait for reply] 
    * @param xd a 3-d vector which contains the desired 
    *          position x,y,z (meters).
    * @param od a 4-d vector which contains the desired 
    *          orientation using axis-angle representation xa, ya,
    *          za, theta (meters and radians).
    * @param xdhat a 3-d vector which is filled with the final 
    *          position x,y,z (meters); it may differ from the
    *          commanded xd.
    * @param odhat a 4-d vector which is filled with the final 
    *          orientation using axis-angle representation xa, ya,
    *          za, theta (meters and radians); it may differ from
    *          the commanded od.
    * @param qdhat the joints configuration through which the
    *             couple (xdhat,odhat) is achieved (degrees).
    * @return true/false on success/failure.
    */
    virtual bool askForPose(const yarp::sig::Vector &xd, const yarp::sig::Vector &od,
                            yarp::sig::Vector &xdhat, yarp::sig::Vector &odhat,
                            yarp::sig::Vector &qdhat)=0;

    /**
    * Ask for inverting a given pose without actually moving there.
    * [wait for reply] 
    * @param q0 a vector of length DOF which contains the starting
    *           joints configuration (degrees), made compatible with
    *           the chain.
    * @param xd a 3-d vector which contains the desired 
    *          position x,y,z (meters).
    * @param od a 4-d vector which contains the desired 
    *          orientation using axis-angle representation xa, ya,
    *          za, theta (meters and radians).
    * @param xdhat a 3-d vector which is filled with the final 
    *          position x,y,z (meters); it may differ from the
    *          commanded xd.
    * @param odhat a 4-d vector which is filled with the final 
    *          orientation using axis-angle representation xa, ya,
    *          za, theta (meters and radians); it may differ from
    *          the commanded od.
    * @param qdhat the joints configuration through which the
    *             couple (xdhat,odhat) is achieved (degrees).
    * @return true/false on success/failure.
    */
    virtual bool askForPose(const yarp::sig::Vector &q0,
                            const yarp::sig::Vector &xd, const yarp::sig::Vector &od,
                            yarp::sig::Vector &xdhat, yarp::sig::Vector &odhat,
                            yarp::sig::Vector &qdhat)=0;

    /**
    * Ask for inverting a given position without actually moving 
    * there. [wait for reply] 
    * @param xd a 3-d vector which contains the desired 
    *          position x,y,z (meters).
    * @param xdhat a 3-d vector which is filled with the final 
    *          position x,y,z (meters); it may differ from the
    *          commanded xd.
    * @param odhat a 4-d vector which is filled with the final 
    *          orientation using axis-angle representation xa, ya,
    *          za, theta (meters and radians); it may differ from
    *          the commanded od.
    * @param qdhat the joints configuration through which the
    *             couple (xdhat,odhat) is achieved (degrees).
    * @return true/false on success/failure.
    */
    virtual bool askForPosition(const yarp::sig::Vector &xd,
                                yarp::sig::Vector &xdhat, yarp::sig::Vector &odhat,
                                yarp::sig::Vector &qdhat)=0;

    /**
    * Ask for inverting a given position without actually moving 
    * there. [wait for reply] 
    * @param q0 a vector of length DOF which contains the starting 
    *           joints configuration (degrees), made compatible with
    *           the chain.
    * @param xd a 3-d vector which contains the desired 
    *          position x,y,z (meters).
    * @param xdhat a 3-d vector which is filled with the final 
    *          position x,y,z (meters); it may differ from the
    *          commanded xd.
    * @param odhat a 4-d vector which is filled with the final 
    *          orientation using axis-angle representation xa, ya,
    *          za, theta (meters and radians); it may differ from
    *          the commanded od.
    * @param qdhat the joints configuration through which the
    *             couple (xdhat,odhat) is achieved (degrees).
    * @return true/false on success/failure.
    */
    virtual bool askForPosition(const yarp::sig::Vector &q0,
                                const yarp::sig::Vector &xd,
                                yarp::sig::Vector &xdhat, yarp::sig::Vector &odhat,
                                yarp::sig::Vector &qdhat)=0;

    /**
    * Get the current DOF configuration of the limb. [wait for
    * reply] 
    * @param curDof a vector which is filled with the actual DOF 
    *           configuration.
    * @return true/false on success/failure. 
    *  
    * @note The vector lenght is equal to the number of limb's 
    *       joints; each vector's position is filled with 1 if the
    *       associated joint is controlled (i.e. it is an actuated
    *       DOF), 0 otherwise.
    */
    virtual bool getDOF(yarp::sig::Vector &curDof)=0;

    /**
    * Set a new DOF configuration for the limb. [wait for reply]
    * @param newDof a vector which contains the new DOF 
    *            configuration.
    * @param curDof a vector where the DOF configuration is 
    *              returned as it has been processed after the
    *              request (it may differ from newDof due to the
    *              presence of some internal limb's constraints).
    * @return true/false on success/failure. 
    *  
    * @note Eeach vector's position shall contain 1 if the 
    *       associated joint can be actuated, 0 otherwise. The
    *       special value 2 indicates that the joint status won't be
    *       modified (useful as a placeholder).
    */
    virtual bool setDOF(const yarp::sig::Vector &newDof, yarp::sig::Vector &curDof)=0;

    /**
    * Get the current joints rest position. [wait for reply]
    * @param curRestPos a vector which is filled with the current 
    *                  joints rest position components in degrees.
    * @return true/false on success/failure. 
    *  
    * @note While solving the inverse kinematic, the user may 
    *       specify a secondary task that minimizes against a joints
    *       rest position; further, each rest component may be
    *       weighted differently providing the weights vector.
    */
    virtual bool getRestPos(yarp::sig::Vector &curRestPos)=0;

    /**
    * Set a new joints rest position. [wait for reply] 
    * @param newRestPos a vector which contains the new joints rest
    *                  position components in degrees.
    * @param curRestPos a vector which is filled with the current 
    *           joints rest position components in degrees as result
    *           from thresholding with joints bounds.
    * @return true/false on success/failure. 
    *  
    * @note While solving the inverse kinematic, the user may 
    *       specify a secondary task that minimizes against a joints
    *       rest position; further, each rest component may be
    *       weighted differently providing the weights vector.
    */
    virtual bool setRestPos(const yarp::sig::Vector &newRestPos, yarp::sig::Vector &curRestPos)=0;

    /**
    * Get the current joints rest weights. [wait for reply]
    * @param curRestWeights a vector which is filled with the 
    *                  current joints rest weights.
    * @return true/false on success/failure. 
    *  
    * @note While solving the inverse kinematic, the user may 
    *       specify a secondary task that minimizes against a joints
    *       rest position; further, each rest component may be
    *       weighted differently providing the weights vector.
    */
    virtual bool getRestWeights(yarp::sig::Vector &curRestWeights)=0;

    /**
    * Set a new joints rest position. [wait for reply] 
    * @param newRestWeights a vector which contains the new joints 
    *                  rest weights.
    * @param curRestWeights a vector which is filled with the 
    *           current joints rest weights as result from
    *           saturation (w>=0.0).
    * @return true/false on success/failure. 
    *  
    * @note While solving the inverse kinematic, the user may 
    *       specify a secondary task that minimizes against a joints
    *       rest position; further, each rest component may be
    *       weighted differently providing the weights vector.
    */
    virtual bool setRestWeights(const yarp::sig::Vector &newRestWeights, yarp::sig::Vector &curRestWeights)=0;

    /**
    * Get the current range for the axis. [wait for reply]
    * @param axis joint index (regardless if it is actuated or 
    *            not).
    * @param min where the minimum value is returned [deg].
    * @param max where the maximum value is returned [deg].
    * @return true/false on success/failure. 
    */
    virtual bool getLimits(const int axis, double *min, double *max)=0;

    /**
    * Set new range for the axis. Allowed range shall be a valid 
    * subset of the real control limits. [wait for reply]
    * @param axis joint index (regardless it it is actuated or 
    *            not).
    * @param min the new minimum value [deg]. 
    * @param max the new maximum value [deg]. 
    * @return true/false on success/failure. 
    */
    virtual bool setLimits(const int axis, const double min, const double max)=0;

    /**
    * Get the current trajectory duration. [wait for reply]
    * @param t time (seconds).
    * @return true/false on success/failure. 
    */
    virtual bool getTrajTime(double *t)=0;

    /**
    * Set the duration of the trajectory. [wait for reply]
    * @param t the memory location where the time is returned 
    *         (seconds).
    * @return true/false on success/failure. 
    */
    virtual bool setTrajTime(const double t)=0;

    /**
    * Return tolerance for in-target check. [wait for reply]
    * @param tol the memory location where tolerance is returned. 
    * @return true/false on success/failure. 
    *  
    * @note The trajectory is supposed to be completed as soon as 
    *       norm(xd-end_effector)<tol.
    */
    virtual bool getInTargetTol(double *tol)=0;

    /**
    * Set tolerance for in-target check. [wait for reply]
    * @param tol tolerance. 
    * @return true/false on success/failure. 
    *  
    * @note The trajectory is supposed to be completed as soon as 
    *       norm(xd-end_effector)<tol.
    */
    virtual bool setInTargetTol(const double tol)=0;

    /**
    * Return joints velocities. [wait for reply] 
    * @param qdot the vector containing the joints velocities 
    *             [deg/s] sent to the robot by the controller.
    * @return true/false on success/failure.
    */
    virtual bool getJointsVelocities(yarp::sig::Vector &qdot)=0;

    /**
    * Return velocities of the end-effector in the task space. [wait
    * for reply] 
    * @param xdot the 3-d vector containing the derivative of x,y,z 
    *             position [m/s] of the end-effector while moving in
    *             the task space as result of the commanded joints
    *             velocities.
    * @param odot the 4-d vector containing the derivative of 
    *             end-effector orientation [rad/s] while moving in
    *             the task space as result of the commanded joints
    *             velocities.
    * @return true/false on success/failure.
    */
    virtual bool getTaskVelocities(yarp::sig::Vector &xdot, yarp::sig::Vector &odot)=0;

    /**
    * Set the reference velocities of the end-effector in the task 
    * space.
    * @param xdot the 3-d vector containing the x,y,z reference 
    *             velocities [m/s] of the end-effector.
    * @param odot the 4-d vector containing the orientation 
    *             reference velocity [rad/s] of the end-effector
    * @return true/false on success/failure.
    */
    virtual bool setTaskVelocities(const yarp::sig::Vector &xdot, const yarp::sig::Vector &odot)=0;

    /** Check once if the current trajectory is terminated. [wait for
    *   reply]
    * @param f where the result is returned.
    * @return true/false on success/failure.
    */
    virtual bool checkMotionDone(bool *f)=0;

    /** Wait until the current trajectory is terminated. [wait for
    *   reply]
    * @param period specify the check time period (seconds). 
    * @param timeout specify the check expiration time (seconds). If
    *         timeout<=0 (as by default) the check will be performed
    *         without time limitation.
    * @return true for success, false for failure and timeout 
    *         expired.
    */
    virtual bool waitMotionDone(const double period=0.1, const double timeout=0.0)=0;

    /** Ask for an immediate stop motion. [wait for reply]
    * @return true/false on success/failure. 
    *  
    * @note The control is completely released, i.e. a direct switch
    *       to non-tracking mode is executed.     
    */
    virtual bool stopControl()=0;

    /** Store the controller context. [wait for reply]
    * @param id specify where to store the returned context id. 
    * @return true/false on success/failure. 
    *  
    * @note The context comprises the values of internal controller 
    *       variables, such as the tracking mode, the active dofs,
    *       the trajectory time and so on.
    */
    virtual bool storeContext(int *id)=0;

    /** Restore the controller context previously stored. [wait for
    *   reply]
    * @param id specify the context id to be restored
    * @return true/false on success/failure. 
    *  
    * @note The context comprises the values of internal controller
    *       variables, such as the tracking mode, the active dofs,
    *       the trajectory time and so on.
    */
    virtual bool restoreContext(const int id)=0;
};

#endif



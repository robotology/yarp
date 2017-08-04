/*
 * Copyright (C) 2017 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Claudio Fantacci
 * CopyPolicy: Released under the terms of the 3-clause BSD.
 */

#ifndef YARP_DEV_VISUALSERVOING_H
#define YARP_DEV_VISUALSERVOING_H

#include <string>
#include <vector>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConstString.h>
#include <yarp/sig/Vector.h>

namespace yarp{
    namespace dev {
        class IVisualServoing;
    }
}


/*!
 * \brief Interface for visual servoing controllers.
 */
class yarp::dev::IVisualServoing
{
public:
    /*!
     * Enumeration class to select left or right camera.
     */
    enum class CamSel { left, right };

    /*!
     * Destructor.
     */
    virtual ~IVisualServoing() { }

    /*!
     * Initialize support classes, modules and connections to perform visual
     * servoing. This method must be called before any other visual servoing
     * methods. Returns upon successful or failure setup.
     *
     * \param use_direct_kin instruct the visual servoing control to either use
     *                       direct kinematic or an estimated/refined pose of
     *                       the end-effector.
     *
     * \note Default value: false. There usually is an error in the robot
     *       direct kinematics that should be compensated to perform precise
     *       visual servoing. To this end, a recursive Bayesian estimation
     *       filter is used to compensate for this error. Such filter is
     *       initialized during initialization execution.
     *
     * \return true/false on success/failure.
     */
    virtual bool initFacilities(bool use_direct_kin) = 0;

    /*!
     * Reset support classes, modules and connections to perform visual
     * servoing. Returns upon successful or failure setup.
     *
     * \note This method also resets the recursive Bayesian estimation filter.
     *       It may happen that the recursive Bayesian filter does not provide
     *       satisfactory pose estimation or diverges. Thus this method can be
     *       used to reset the filter.
     *
     * \return true/false on success/failure.
     */
    virtual bool resetFacilities() = 0;

    /*!
     * Deallocate support classes, stop modules and disconnect connections used
     * for visual servoing. This method must be called when visual servoing is
     * no longer needed or a new visual servoing instance is needed.
     *
     * \note This method also stops the recursive Bayesian estimation filter.
     *       Thus it is suggested to call this method every time visual servoing
     *       has been completed/interrupted to have the filter stopped and
     *       initialized again during the next init call.
     *
     * \return true/false on success/failure.
     */
    virtual bool stopFacilities() = 0;

    /*!
     * Set the goal points on both left and right camera image plane and start
     * visual servoing.
     *
     * \param vec_px_l a collection of four 2D vectors which contains the (u,v)
     *                 coordinates of the pixels within the left image plane.
     * \param vec_px_r a collection of four 2D vectors which contains the (u,v)
     *                 coordinates of the pixels within the right image plane.
     *
     * \note By invoking this method, the visual servoing goal will be reached in
     *       orientation first, then in position. This is because there may not
     *       be a feasible position solution for every possible orientation.
     *
     * \return true/false on success/failure.
     */
    virtual bool goToGoal(const std::vector<yarp::sig::Vector>& vec_px_l, const std::vector<yarp::sig::Vector>& vec_px_r) = 0;

    /*!
     * Set the goal point (3D for the position + 4D axis-angle for
     * the orientation) and start visual servoing.
     *
     * \param vec_x a 3D vector which contains the (x, y, z) Cartesian
     *              coordinates of the goal.
     * \param vec_o a 4D vector which contains the (x, y, z) axis and theta angle
     *              of rotation of the goal.
     *
     * \note By invoking this method, the visual servoing goal will be reached in
     *       position and orientation together with two parallel tasks.
     *
     * \return true/false on success/failure.
     */
    virtual bool goToGoal(const yarp::sig::Vector& vec_x, const yarp::sig::Vector& vec_o) = 0;

    /*!
     *  Set visual servoing operating mode between:
     *  1. 'position': position-only visual servo control;
     *  2. 'orientation': orientation-only visual servo control;
     *  3. 'pose': position + orientation visual servo control.
     *
     * \param mode a label referring to one of the three operating mode, i.e.
     *             'position', 'orientation' or 'pose'.
     *
     * \return true/false on success/failure.
     */
    virtual bool setModality(const std::string& mode) = 0;

    /*!
     * Set the point controlled during visual servoing.
     *
     * \param point label of the point to control.
     *
     * \return true/false on success/failure.
     *
     * \note The points available to control are identified by a distinct,
     *       unique label. Such labels can are stored in the bottle returned by
     *       the getInfo() method.
     */
    virtual bool setControlPoint(const yarp::os::ConstString& point) = 0;

    /*!
     * Return useful information for visual servoing.
     *
     * \param info YARP Bottle cotaining all the visual servoing information.
     *
     * \return true/false on success/failure.
     */
    virtual bool getVisualServoingInfo(yarp::os::Bottle& info) = 0;

    /*!
     * Set visual servoing goal tolerance.
     *
     * \param tol the tolerance in pixel.
     *
     * \return true/false on success/failure.
     *
     * \note Default value: 15.0 [pixel].
     */
    virtual bool setGoToGoalTolerance(const double tol = 15.0) = 0;

    /*!
     * Check once whether the visual servoing controller is running or not.
     *
     * \return true/false on it is running/not running.
     *
     * \note The visual servoing controller may be terminated due to many
     *       different reasons, not strictly related to reaching the goal.
     */
    virtual bool checkVisualServoingController() = 0;

    /*!
     * Wait until visual servoing reaches the goal.
     * [wait for reply]
     *
     * \param period the check time period [s].
     * \param timeout the check expiration time [s]. If timeout <= 0 (as by
     *                default) the check will be performed without time
     *                limitation.
     *
     * \return true for success, false for failure and timeout expired.
     *
     * \note The tolerance to which the goal is considered achieved can be set
     *       with the method setGoToGoalTolerance().
     */
    virtual bool waitVisualServoingDone(const double period = 0.1, const double timeout = 0.0) = 0;

    /*!
     * Ask for an immediate stop of the visual servoing controller.
     * [wait for reply]
     *
     * \return true/false on success/failure.
     */
    virtual bool stopController() = 0;

    /*!
     * Set the translation gains of the visual servoing control algorithm. The
     * two values are used, respectively, when the end-effector is far away from
     * and close to the goal.
     *
     * \return true/false on success/failure.
     *
     * \note Warning: higher values of the gain corresponds to higher
     *       translation velocities and oscillation about the goal.
     *
     * \note Default values: K_x_1 = 1.0, K_x_2 = 0.25.
     */
    virtual bool setTranslationGain(const double K_x_1 = 1.0, const double K_x_2 = 0.25) = 0;

    /*!
     * Set the maximum translation velocity of the visual servoing control
     * algorithm (same for each axis).
     *
     * \param max_x_dot the maximum allowed velocity for x, y, z coordinates
     *                  [m/s].
     *
     * \return true/false on success/failure.
     */
    virtual bool setMaxTranslationVelocity(const double max_x_dot) = 0;

    /*!
     * Set the tolerance, in pixels, at which the translation control law
     * swithces its gain value.
     *
     * \return true/false on success/failure.
     *
     * \note Default value: K_x_tol = 30.0 [pixel].
     */
    virtual bool setTranslationGainSwitchTolerance(const double K_x_tol = 30.0) = 0;

    /*!
     * Set the orientation gains of the visual servoing control algorithm. The
     * two values are used, respectively, when the end-effector is far away from
     * and close to the goal.
     *
     * @return true/false on success/failure.
     *
     * @note Warning: higher values of the gain corresponds to higher
     *       translation velocities and oscillation about the goal.
     *
     * @note Default values: K_o_1 = 1.5, K_o_2 = 0.375.
     */
    virtual bool setOrientationGain(const double K_o_1 = 1.5, const double K_o_2 = 0.375) = 0;

    /*!
     * Set the maximum angular velocity of the axis-angle velocity vector of the
     * visual servoing control algorithm.
     *
     * \param max_x_dot the maximum allowed angular velocity [rad/s].
     *
     * \return true/false on success/failure.
     */
    virtual bool setMaxOrientationVelocity(const double max_o_dot) = 0;

    /**
     * Set the tolerance, in pixels, at which the orientation control law
     * swithces its gain value.
     *
     * \return true/false on success/failure.
     *
     * \note Default value: K_o_tol = 30.0 [pixel].
     */
    virtual bool setOrientationGainSwitchTolerance(const double K_o_tol = 30.0) = 0;

    /*!
     * Helper function: extract four Cartesian points lying on the plane defined
     * by the frame o in the position x relative to the robot base frame.
     *
     * \param x a 3D vector which is filled with the actual position x,y,z [m].
     * \param o a 4D vector which is filled with the actual orientation using
     *          axis-angle representation xa, ya, za, theta [rad].
     *
     * \return on success: a collection of four Cartesian points (position only)
     *         extracted from the plane defined by x and o;
     *         on failure: an empty list.
     *
     * \note It is always suggested to check whether the returned list is empty
     *       or not and to take proper counter actions.
     */
    virtual std::vector<yarp::sig::Vector> get3DGoalPositionsFrom3DPose(const yarp::sig::Vector& x, const yarp::sig::Vector& o) = 0;

    /*!
     * Helper function: extract four 2D pixel points lying on the plane defined
     * by the frame o in the position x relative to the robot base frame.
     *
     * \param x a 3D vector which is filled with the actual position x,y,z [m].
     * \param o a 4D vector which is filled with the actual orientation using
     *          axis-angle representation xa, ya, za, theta [m]/[rad].
     * \param cam either "left" or "right" to select left or right camera.
     *
     * \return on success: a collection of three (u, v) pixel points
     *         extracted from the plane defined by x and o;
     *         on failure: an empty list.
     *
     * \note It is always suggested to check whether the returned list is empty
     *       or not and to take proper counter actions.
     */
    virtual std::vector<yarp::sig::Vector> getGoalPixelsFrom3DPose(const yarp::sig::Vector& x, const yarp::sig::Vector& o, const CamSel& cam) = 0;


    /* TO BE DEPRECATED */
    /*!
     * Initialize the robot to an initial position.
     * The initial positions are stored on an external file and are referenced
     * by a unique label.
     *
     * \param label a label referring to one of the available initial positions;
     *              the string shall be one of the available modes returned
     *              by the get_info() method.
     *
     * \return true upon success, false otherwise.
     */
    virtual bool storedInit(const std::string& label) = 0;

    /*!
     * Set the robot visual servoing goal.
     * The goals are stored on an external file and are referenced by a unique
     * label.
     *
     * \param label a label referring to one of the available goals;
     *              the string shall be one of the available modes returned
     *              by the get_info() method.
     *
     * \return true upon success, false otherwise.
     */
    virtual bool storedGoToGoal(const std::string& label) = 0;

    /*!
     * Get goal point from SFM module. The point is taken by clicking on a
     * dedicated 'yarpview' GUI and the orientation is hard-coded.
     *
     * \note This service is experimental and should be used with care.
     *
     * \return true upon success, false otherwise.
     */
    virtual bool goToSFMGoal() = 0;
};

#endif /* YARP_DEV_VISUALSERVOING_H */

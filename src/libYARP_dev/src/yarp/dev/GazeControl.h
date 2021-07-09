/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_GAZECONTROL_H
#define YARP_DEV_GAZECONTROL_H

#include <string>

#include <yarp/os/Bottle.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/api.h>
#include <yarp/sig/Vector.h>

/*!
 * \file GazeControl.h defines gaze control interfaces
 */

namespace yarp {
    namespace dev {
        struct GazeEventParameters;
        struct GazeEventVariables;
        class  GazeEvent;
        class  IGazeControl;
    }
}


/*!
 * \ingroup dev_iface_motor
 *
 * \brief Structure for configuring a gaze event.
 */
struct YARP_dev_API yarp::dev::GazeEventParameters
{
    /*!
     * The signature of the event as specified by the user.
     *
     * Available events are:
     *  - "motion-onset": beginning of motion/new target received.
     *  - "motion-done": end of motion.
     *  - "motion-ongoing": a motion check-point is attained.
     *  - "saccade-onset": beginning of saccade.
     *  - "saccade-done": end of saccade.
     *  - "stabilization-on": stabilization is turned on.
     *  - "stabilization-off": stabilization is turned off.
     *  - "closing": the server is being shut down.
     *  - "suspended": the server has been suspeded.
     *  - "resumed": the server has been resumed.
     *  - "comm-timeout": the serve has been suspended because of a
     *    communication timeout.
     *  - "*": a tag for all-events.
     */
    std::string type;

    /*!
     * The user specifies the motion check-point that raises a
     * "motion-ongoing" event through this parameter which must be
     * in the range [0,1].
     */
    double motionOngoingCheckPoint;
};


/*!
 * \ingroup dev_iface_motor
 *
 * \brief Structure for retrieving information from a gaze event.
 */
struct YARP_dev_API yarp::dev::GazeEventVariables
{
    /*!
     * The signature of the received event as filled by the event
     * handler.
     */
    std::string type;

    /*!
     * Contain the time instant of the source when the event took
     * place, as filled by the event handler.
     */
    double time;

    /*!
     * Contain the motion check-point that raised a "motion-ongoing"
     * event.
     */
    double motionOngoingCheckPoint;
};


/*!
 * \ingroup dev_iface_motor
 *
 * \brief Interface for a event notified by the gaze controller.
 */
class YARP_dev_API yarp::dev::GazeEvent
{
public:
    /*!
     * Destructor.
     */
    virtual ~GazeEvent() {}

    /*!
     * The user fills this structure to establish the event
     * parameters.
     */
    yarp::dev::GazeEventParameters gazeEventParameters;

    /*!
     * The event handler fills this structure with useful
     * information at run-time.
     */
    yarp::dev::GazeEventVariables gazeEventVariables;

    /*!
     * Event callback to be overridden by the user.
     */
    virtual void gazeEventCallback() = 0;
};


/*!
 * \ingroup dev_iface_motor
 *
 * \brief Interface for a gaze controller.
 *
 * Please read carefully the
 * <a href="http://wiki.icub.org/iCub/main/dox/html/icub_gaze_interface.html">documentation</a>.
 */
class YARP_dev_API yarp::dev::IGazeControl
{
public:
    /*!
     * Destructor.
     */
    virtual ~IGazeControl();

    /*!
     * Set the controller in tracking or non-tracking mode. [wait for
     * reply]
     * \param f true for tracking mode, false otherwise.
     * \return true/false on success/failure.
     *
     * \note In tracking mode when the controller reaches the target,
     *       it keeps on running in order to maintain the gaze on
     *       target in the presence of external disturbances. In
     *       non-tracking mode the controller releases the control of
     *       the head as soon as the desired target is reached.
     */
    virtual bool setTrackingMode(const bool f) = 0;

    /*!
     * Get the current controller mode. [wait for reply]
     * \param f here is returned true if controller is in tracking
     *         mode, false otherwise.
     * \return true/false on success/failure.
     */
    virtual bool getTrackingMode(bool *f) = 0;

    /*!
     * Turn on/off the gaze stabilization. [wait for reply]
     * \param f true to turn on the stabilization, false otherwise.
     * \return true/false on success/failure.
     *
     * \note When this mode is turned on, the controller continuously
     *       stabilizes the gaze relying on the inertial data.
     */
    virtual bool setStabilizationMode(const bool f) = 0;

    /*!
     * Get the current stabilization mode. [wait for reply]
     * \param f here is returned true if controller is stabilizing the gaze,
     *         false otherwise.
     * \return true/false on success/failure.
     */
    virtual bool getStabilizationMode(bool *f) = 0;

    /*!
     * Get the current fixation point. [do not wait for reply]
     * \param fp a 3-d vector which is filled with the actual
     *         fixation point x,y,z [m].
     * \param stamp the stamp of the encoders employed to compute the
     *              pose.
     * \return true/false on success/failure.
     */
    virtual bool getFixationPoint(yarp::sig::Vector &fp, yarp::os::Stamp *stamp = NULL) = 0;

    /*!
     * Get the current gaze configuration in terms of azimuth and
     * elevation angles wrt to the absolute reference frame along
     * with the vergence. All angles are expressed in degrees. [do
     * not wait for reply]
     * \param ang a 3-d vector which is filled with the actual
     *         angles azimuth/elevation/vergence (degrees).
     * \param stamp the stamp of the encoders employed to compute the
     *              pose.
     * \return true/false on success/failure.
     *
     * \note The absolute reference frame for the azimuth/elevation
     *       couple is head-centered with the robot in rest
     *       configuration (i.e. torso and head angles zeroed).
     */
    virtual bool getAngles(yarp::sig::Vector &ang, yarp::os::Stamp *stamp = NULL) = 0;

    /*!
     * Move the gaze to a specified fixation point in cartesian
     * space. [do not wait for reply]
     * \param fp a 3-d vector which contains the desired fixation
     *         point where to look at x,y,z.
     * \return true/false on success/failure.
     */
    virtual bool lookAtFixationPoint(const yarp::sig::Vector &fp) = 0;

    /*!
     * Move the gaze to a specified gazing angles configuration given
     * in the absolute reference frame. [do not wait for reply]
     * \param ang a 3-d vector which contains the desired gazing
     *         angles (azimuth/elevation/vergence) expressed in the
     *         absolute reference frame (degrees).
     * \return true/false on success/failure.
     *
     * \note The absolute reference frame for the azimuth/elevation
     *       couple is head-centered with the robot in rest
     *       configuration (i.e. torso and head angles zeroed).
     */
    virtual bool lookAtAbsAngles(const yarp::sig::Vector &ang) = 0;

    /*!
     * Move the gaze to a specified gazing angles configuration given
     * in the relative reference frame. [do not wait for reply]
     * \param ang a 3-d vector which contains the desired gazing
     *         angles (azimuth/elevation/vergence) expressed in the
     *         relative reference frame (degrees).
     * \return true/false on success/failure.
     *
     * \note The relative reference frame for the azimuth/elevation
     *       couple is head-centered; the center of this frame is
     *       located in the middle of the baseline that connects the
     *       two eyes.
     */
    virtual bool lookAtRelAngles(const yarp::sig::Vector &ang) = 0;

    /*!
     * Move the gaze to a location specified by a pixel within the
     * image plane. [do not wait for reply]
     * \param camSel selects the image plane: 0 for the left, 1 for
     *              the right.
     * \param px a 2-d vector which contains the (u,v) coordinates of
     *           the pixel within the image plane.
     * \param z the z-component of the point in the eye's reference
     *         frame [m]. A default value of 1.0 is assumed.
     * \return true/false on success/failure.
     *
     * \note The component z can be seen also as the distance [m]
     *       from the proper image plane once thought to extend
     *       towards infinity.
     */
    virtual bool lookAtMonoPixel(const int camSel,
                                 const yarp::sig::Vector &px,
                                 const double z = 1.0) = 0;

    /*!
     * Move the gaze to a location specified by a pixel within the
     * image plane using the vergence. [do not wait for reply]
     * \param camSel selects the image plane: 0 for the left, 1 for
     *              the right.
     * \param px a 2-d vector which contains the (u,v) coordinates of
     *           the pixel within the image plane.
     * \param ver the vergence angle given in degrees.
     * \return true/false on success/failure.
     */
    virtual bool lookAtMonoPixelWithVergence(const int camSel,
                                             const yarp::sig::Vector &px,
                                             const double ver) = 0;

    /*!
     * Move the gaze to a location specified by two pixels
     * representing the same 3-d point as seen from within both image
     * planes. [do not wait for reply]
     * \param pxl a 2-d vector which contains the (u,v) coordinates
     *           of the pixel within the left image plane.
     * \param pxr a 2-d vector which contains the (u,v) coordinates
     *           of the pixel within the right image plane.
     * \return true/false on success/failure.
     *
     * \note This strategy employs the monocular approach coupled
     *       with a pid that varies the distance z incrementally
     *       according to the actual error: to achieve the target it
     *       is thus required to provide the visual feedback
     *       continuously.
     */
    virtual bool lookAtStereoPixels(const yarp::sig::Vector &pxl,
                                    const yarp::sig::Vector &pxr) = 0;

    /*!
     * Move the gaze to a specified fixation point in cartesian
     * space. [wait for reply]
     * \param fp a 3-d vector which contains the desired fixation
     *         point where to look at x,y,z.
     * \return true/false on success/failure.
     */
    virtual bool lookAtFixationPointSync(const yarp::sig::Vector &fp) = 0;

    /*!
     * Move the gaze to a specified gazing angles configuration given
     * in the absolute reference frame. [wait for reply]
     * \param ang a 3-d vector which contains the desired gazing
     *         angles (azimuth/elevation/vergence) expressed in the
     *         absolute reference frame (degrees).
     * \return true/false on success/failure.
     *
     * \note The absolute reference frame for the azimuth/elevation
     *       couple is head-centered with the robot in rest
     *       configuration (i.e. torso and head angles zeroed).
     */
    virtual bool lookAtAbsAnglesSync(const yarp::sig::Vector &ang) = 0;

    /*!
     * Move the gaze to a specified gazing angles configuration given
     * in the relative reference frame. [wait for reply]
     * \param ang a 3-d vector which contains the desired gazing
     *         angles (azimuth/elevation/vergence) expressed in the
     *         relative reference frame (degrees).
     * \return true/false on success/failure.
     *
     * \note The relative reference frame for the azimuth/elevation
     *       couple is head-centered; the center of this frame is
     *       located in the middle of the baseline that connects the
     *       two eyes.
     */
    virtual bool lookAtRelAnglesSync(const yarp::sig::Vector &ang) = 0;

    /*!
     * Move the gaze to a location specified by a pixel within the
     * image plane. [wait for reply]
     * \param camSel selects the image plane: 0 for the left, 1 for
     *              the right.
     * \param px a 2-d vector which contains the (u,v) coordinates of
     *           the pixel within the image plane.
     * \param z the z-component of the point in the eye's reference
     *         frame [m]. A default value of 1.0 is assumed.
     * \return true/false on success/failure.
     *
     * \note The component z can be seen also as the distance [m]
     *       from the proper image plane once thought to extend
     *       towards infinity.
     */
    virtual bool lookAtMonoPixelSync(const int camSel,
                                     const yarp::sig::Vector &px,
                                     const double z = 1.0) = 0;

    /*!
     * Move the gaze to a location specified by a pixel within the
     * image plane using the vergence. [wait for reply]
     * \param camSel selects the image plane: 0 for the left, 1 for
     *              the right.
     * \param px a 2-d vector which contains the (u,v) coordinates of
     *           the pixel within the image plane.
     * \param ver the vergence angle given in degrees.
     * \return true/false on success/failure.
     */
    virtual bool lookAtMonoPixelWithVergenceSync(const int camSel,
                                                 const yarp::sig::Vector &px,
                                                 const double ver) = 0;

    /*!
     * Move the gaze to a location specified by two pixels
     * representing the same 3-d point as seen from within both image
     * planes. [wait for reply]
     * \param pxl a 2-d vector which contains the (u,v) coordinates
     *           of the pixel within the left image plane.
     * \param pxr a 2-d vector which contains the (u,v) coordinates
     *           of the pixel within the right image plane.
     * \return true/false on success/failure.
     *
     * \note This strategy employs the monocular approach coupled
     *       with a pid that varies the distance z incrementally
     *       according to the actual error: to achieve the target it
     *       is thus required to provide the visual feedback
     *       continuously.
     */
    virtual bool lookAtStereoPixelsSync(const yarp::sig::Vector &pxl,
                                        const yarp::sig::Vector &pxr) = 0;

    /*!
     * Get the current trajectory duration for the neck actuators.
     * [wait for reply]
     * \param t time (seconds).
     * \return true/false on success/failure.
     */
    virtual bool getNeckTrajTime(double *t) = 0;

    /*!
     * Get the current trajectory duration for the eyes actuators.
     * [wait for reply]
     * \param t time (seconds).
     * \return true/false on success/failure.
     */
    virtual bool getEyesTrajTime(double *t) = 0;

    /*!
     * Get the current gain used to weigh the vestibulo-ocular reflex
     * (VOR). [wait for reply]
     * \param gain the current gain.
     * \return true/false on success/failure.
     */
    virtual bool getVORGain(double *gain) = 0;

    /*!
     * Get the current gain used to weigh the oculo-collic reflex
     * (OCR). [wait for reply]
     * \param gain the current gain.
     * \return true/false on success/failure.
     */
    virtual bool getOCRGain(double *gain) = 0;

    /*!
     * Query whether the very fast eyes movements (saccades) will be
     * employed or not. [wait for reply]
     * \param f the current saccades mode.
     * \return true/false on success/failure.
     */
    virtual bool getSaccadesMode(bool *f) = 0;

    /*!
     * Get the current inhibition period for saccadic movements.
     * [wait for reply]
     * \param period the current inhibition period [s].
     * \return true/false on success/failure.
     */
    virtual bool getSaccadesInhibitionPeriod(double *period) = 0;

    /*!
     * Get the current activation angle for saccadic movements. [wait
     * for reply]
     * \param angle the current activation angle [deg].
     * \return true/false on success/failure.
     */
    virtual bool getSaccadesActivationAngle(double *angle) = 0;

    /*!
     * Get the current pose of the left eye frame. [wait for reply]
     * \param x a 3-d vector which is filled with the actual
     *         position x,y,z [m].
     * \param o a 4-d vector which is filled with the actual
     * orientation using axis-angle representation xa, ya, za, theta
     * [m]/[rad].
     * \param stamp the stamp of the encoders employed to compute the
     *              pose.
     * \return true/false on success/failure.
     */
    virtual bool getLeftEyePose(yarp::sig::Vector &x, yarp::sig::Vector &o,
                                yarp::os::Stamp *stamp = NULL) = 0;

    /*!
     * Get the current pose of the right eye frame. [wait for reply]
     * \param x a 3-d vector which is filled with the actual
     *         position x,y,z [m].
     * \param o a 4-d vector which is filled with the actual
     * orientation using axis-angle representation xa, ya, za, theta
     * [m]/[rad].
     * \param stamp the stamp of the encoders employed to compute the
     *              pose.
     * \return true/false on success/failure.
     */
    virtual bool getRightEyePose(yarp::sig::Vector &x, yarp::sig::Vector &o,
                                 yarp::os::Stamp *stamp = NULL) = 0;

    /*!
     * Get the current pose of the head frame. [wait for reply]
     * \param x a 3-d vector which is filled with the actual
     *         position x,y,z [m].
     * \param o a 4-d vector which is filled with the actual
     * orientation using axis-angle representation xa, ya, za, theta
     * [m]/[rad].
     * \param stamp the stamp of the encoders employed to compute the
     *              pose.
     * \return true/false on success/failure.
     *
     * \note The center of the head frame is located in the middle of
     *       the baseline that connects the two eyes. The orientation
     *       of its frame is fixed with respect to the head with
     *       z-axis pointing forward, x-axis pointing rightward and
     *       y-axis pointing downward.
     */
    virtual bool getHeadPose(yarp::sig::Vector &x, yarp::sig::Vector &o,
                             yarp::os::Stamp *stamp = NULL) = 0;

    /*!
     * Get the 2-d pixel point - whose cartesian coordinates are
     * given wrt the root reference frame - that results from the
     * projection into the image plane. [wait for reply]
     * \param camSel selects the image plane: 0 for the left, 1 for
     *              the right.
     * \param x the 3-d point given wrt the root reference frame [m].
     * \param px the returned 2-d vector which contains the (u,v)
     *           coordinates of the pixel within the image plane.
     * \return true/false on success/failure.
     */
    virtual bool get2DPixel(const int camSel,
                            const yarp::sig::Vector &x,
                            yarp::sig::Vector &px) = 0;

    /*!
     * Get the 3-d point whose pixel projection and z-component in
     * the camera frame are given. [wait for reply]
     * \param camSel selects the image plane: 0 for the left, 1 for
     *              the right.
     * \param px a 2-d vector which contains the (u,v) coordinates of
     *           the pixel within the image plane.
     * \param z the z-component of the point in the eye's reference
     *         frame [m].
     * \param x the returned 3-d point given wrt the root reference
     *          frame [m].
     * \return true/false on success/failure.
     */
    virtual bool get3DPoint(const int camSel,
                            const yarp::sig::Vector &px,
                            const double z,
                            yarp::sig::Vector &x) = 0;

    /*!
     * Get the 3-d point - whose pixel projection is given - that
     * results from the projection into a plane specified in the
     * space. [wait for reply]
     * \param camSel selects the image plane: 0 for the left, 1 for
     *              the right.
     * \param px a 2-d vector which contains the (u,v) coordinates of
     *           the pixel within the image plane.
     * \param plane the 4-d vector containing the components
     *              (a,b,c,d) of the plane expressed wrt the root
     *              reference frame in its implicit form as:
     *              ax+by+cz+d=0.
     * \param x the returned 3-d point given wrt the root reference
     *          frame [m].
     * \return true/false on success/failure.
     */
    virtual bool get3DPointOnPlane(const int camSel,
                                   const yarp::sig::Vector &px,
                                   const yarp::sig::Vector &plane,
                                   yarp::sig::Vector &x) = 0;

    /*!
     * Get the 3-d coordinates corresponding to the angular
     * positions. [wait for reply]
     * \param mode selects the angular mode: 0 for absolute, 1 for
     *             relative mode.
     * \param ang a 3-d vector which contains the actual angles
     *            azimuth/elevation/vergence (degrees).
     * \param x the returned 3-d point given wrt the root reference
     *          frame [m].
     * \return true/false on success/failure.
     */
    virtual bool get3DPointFromAngles(const int mode,
                                      const yarp::sig::Vector &ang,
                                      yarp::sig::Vector &x) = 0;

    /*!
     * Get the absolute angular coordinates corresponding to the 3-d
     * point. [wait for reply]
     * \param x the 3-d point given wrt the root reference frame [m].
     * \param ang a 3-d vector which is filled with the
     *            azimuth/elevation/vergence coordinates in the
     *            absolute mode (degrees).
     * \return true/false on success/failure.
     *
     * \note Only absolute angular frame can be referred.
     */
    virtual bool getAnglesFrom3DPoint(const yarp::sig::Vector &x,
                                      yarp::sig::Vector &ang) = 0;

    /*!
     * Triangulate two points given in the image planes to find the
     * corresponding 3-d point in the space. [wait for reply]
     * \param pxl a 2-d vector which contains the (u,v) coordinates
     *           of the pixel within the left image plane.
     * \param pxr a 2-d vector which contains the (u,v) coordinates
     *           of the pixel within the right image plane.
     * \param x the returned 3-d point given wrt the root reference
     *          frame [m].
     * \return true/false on success/failure.
     *
     * \note The triangulation makes use of the <a href="http://en.wikipedia.org/wiki/Triangulation_(computer_vision)">
     * mid-point method</a> and thus is deeply affected by uncertainties in the
     * cameras extrinsic parameters and cameras alignment.
     */
    virtual bool triangulate3DPoint(const yarp::sig::Vector &pxl,
                                    const yarp::sig::Vector &pxr,
                                    yarp::sig::Vector &x) = 0;

    /*!
     * Get the joints target values where the controller is moving
     * the system to. [wait for reply]
     * \param qdes a vector which is filled with the desired joints
     *         values (degrees).
     * \return true/false on success/failure.
     */
    virtual bool getJointsDesired(yarp::sig::Vector &qdes) = 0;

    /*!
     * Get the joints velocities commanded by the controller. [wait
     * for reply]
     * \param qdot a vector which is filled with the joints
     *         velocities (deg/s).
     * \return true/false on success/failure.
     */
    virtual bool getJointsVelocities(yarp::sig::Vector &qdot) = 0;

    /*!
     * Return the current options used by the stereo approach. [wait
     * for reply]
     * \param options is a property-like bottle containing the
     *                current configuration employed by the internal
     *                pid.
     * \return true/false on success/failure.
     *
     * \note The returned bottle looks like as follows:
     * (Kp (1 2 ...)) (Ki (1 2 ...)) (Kd (1 2 ...)) (Wp (...)) ...
     * \note the satLim property is returned ordered by rows.
     */
    virtual bool getStereoOptions(yarp::os::Bottle &options) = 0;

    /*!
     * Set the duration of the trajectory for the neck actuators.
     * [wait for reply]
     * \param t time (seconds).
     * \return true/false on success/failure.
     *
     * \note The neck movements time cannot be set equal or lower
     *       than the eyes movements time.
     */
    virtual bool setNeckTrajTime(const double t) = 0;

    /*!
     * Set the duration of the trajectory for the eyes actuators.
     * [wait for reply]
     * \param t time (seconds).
     * \return true/false on success/failure.
     */
    virtual bool setEyesTrajTime(const double t) = 0;

    /*!
     * Set the gain used to weigh the vestibulo-ocular reflex (VOR).
     * [wait for reply]
     * \param gain the new gain.
     * \return true/false on success/failure.
     *
     * \note To disable VOR set gain equal to 0.0.
     */
    virtual bool setVORGain(const double gain) = 0;

    /*!
     * Set the gain used to weigh the oculo-collic reflex (OCR).
     * [wait for reply]
     * \param gain the new gain.
     * \return true/false on success/failure.
     *
     * \note To disable OCR set gain equal to 0.0.
     */
    virtual bool setOCRGain(const double gain) = 0;

    /*!
     * Enable/disable the use of very fast eyes movements (saccades).
     * [wait for reply]
     * \param f true/false to enable/disable saccades.
     * \return true/false on success/failure.
     *
     * \note Vision processing algorithms might be heavily affected
     *       by saccades.
     */
    virtual bool setSaccadesMode(const bool f) = 0;

    /*!
     * Set the inhibition period for saccadic movements. [wait for
     * reply]
     * \param period the new inhibition period [s].
     * \return true/false on success/failure.
     */
    virtual bool setSaccadesInhibitionPeriod(const double period) = 0;

    /*!
     * Set the activation angle for saccadic movements. [wait for
     * reply]
     * \param angle the new activation angle [deg].
     * \return true/false on success/failure.
     */
    virtual bool setSaccadesActivationAngle(const double angle) = 0;

    /*!
     * Update the options used by the stereo approach. [wait for
     * reply]
     * \param options is a property-like bottle containing the new
     *                configuration employed by the internal pid.
     * \return true/false on success/failure.
     *
     * \note The property parameter should look like as follows:
     * (Kp (1 2 ...)) (Ki (1 2 ...)) (Kd (1 2 ...)) (Wp (...)) ...
     * \note The vectors dimension at pid creation time is always
     *       retained.
     * \note The satLim property must be given ordered by rows.
     * \note The sample time Ts should match the rate with which the
     *       method lookAtStereoPixels is called by the user.
     * \note The special option "dominantEye" (that can be "left"|"right")
     *       allows selecting the eye used for the monocular approach.
     */
    virtual bool setStereoOptions(const yarp::os::Bottle &options) = 0;

    /*!
     * Bind the neck pitch within a specified range. [wait for
     *   reply]
     * \param min the minimum value of the range (in degrees).
     * \param max the maximum value of the range (in degrees).
     * \return true/false on success/failure.
     */
    virtual bool bindNeckPitch(const double min, const double max) = 0;

    /*!
     * Block the neck pitch at a specified angle. [wait for reply]
     * \param val the angle value at which block the joint (in
     *           degrees).
     * \return true/false on success/failure.
     *
     * \note The possibility to block the neck joints is given in
     *       order to move just the eyes.
     */
    virtual bool blockNeckPitch(const double val) = 0;

    /*!
     * Block the neck pitch at the current angle. [wait for reply]
     * \return true/false on success/failure.
     *
     * \note The possibility to block the neck joints is given in
     *       order to move just the eyes.
     */
    virtual bool blockNeckPitch() = 0;

    /*!
     * Bind the neck roll within a specified range. [wait for
     *   reply]
     * \param min the minimum value of the range (in degrees).
     * \param max the maximum value of the range (in degrees).
     * \return true/false on success/failure.
     */
    virtual bool bindNeckRoll(const double min, const double max) = 0;

    /*!
     * Block the neck roll at a specified angle. [wait for reply]
     * \param val the angle value at which block the joint (in
     *           degrees).
     * \return true/false on success/failure.
     *
     * \note The possibility to block the neck joints is given in
     *       order to move just the eyes.
     */
    virtual bool blockNeckRoll(const double val) = 0;

    /*!
     * Block the neck roll at the current angle. [wait for reply]
     * \return true/false on success/failure.
     *
     * \note The possibility to block the neck joints is given in
     *       order to move just the eyes.
     */
    virtual bool blockNeckRoll() = 0;

    /*!
     * Bind the neck yaw within a specified range. [wait for
     *   reply]
     * \param min the minimum value of the range (in degrees).
     * \param max the maximum value of the range (in degrees).
     * \return true/false on success/failure.
     */
    virtual bool bindNeckYaw(const double min, const double max) = 0;

    /*!
     * Block the neck yaw at a specified angle. [wait for reply]
     * \param val the angle value at which block the joint (in
     *           degrees).
     * \return true/false on success/failure.
     *
     * \note The possibility to block the neck joints is given in
     *       order to move just the eyes.
     */
    virtual bool blockNeckYaw(const double val) = 0;

    /*!
     * Block the neck yaw at the current angle. [wait for reply]
     * \return true/false on success/failure.
     *
     * \note The possibility to block the neck joints is given in
     *       order to move just the eyes.
     */
    virtual bool blockNeckYaw() = 0;

    /*!
     * Block the eyes to look always straight ahead
     *   (the tilt can vary) with a specified vergence
     *   angle. [wait for reply]
     * \param ver the vergence angle (in degrees).
     * \return true/false on success/failure.
     */
    virtual bool blockEyes(const double ver) = 0;

    /*! Block the eyes to look always straight ahead
     *   (the tilt can vary) with the current vergence
     *   angle. [wait for reply]
     * \return true/false on success/failure.
     */
    virtual bool blockEyes() = 0;

    /*!
     * Return the current neck pitch range. [wait for reply]
     * \param min the location where to store the minimum of the
     *            range [deg].
     * \param max the location where to store the maximum of the
     *            range [deg].
     * \return true/false on success/failure.
     */
    virtual bool getNeckPitchRange(double *min, double *max) = 0;

    /*!
     * Return the current neck roll range. [wait for reply]
     * \param min the location where to store the minimum of the
     *            range [deg].
     * \param max the location where to store the maximum of the
     *            range [deg].
     * \return true/false on success/failure.
     */
    virtual bool getNeckRollRange(double *min, double *max) = 0;

    /*!
     * Return the current neck yaw range. [wait for reply]
     * \param min the location where to store the minimum of the
     *            range [deg].
     * \param max the location where to store the maximum of the
     *            range [deg].
     * \return true/false on success/failure.
     */
    virtual bool getNeckYawRange(double *min, double *max) = 0;

    /*!
     * Return the current vergence angle used to block the eyes.
     *   [wait for reply]
     * \param ver the vergence angle [deg].
     * \return true/false on success/failure.
     *
     * \note negative values of returned vergence correspond to
     *       unblocked eyes.
     */
    virtual bool getBlockedVergence(double *ver) = 0;

    /*!
     * Unblock the neck pitch. [wait for reply]
     * \return true/false on success/failure.
     */
    virtual bool clearNeckPitch() = 0;

    /*!
     * Unblock the neck roll. [wait for reply]
     * \return true/false on success/failure.
     */
    virtual bool clearNeckRoll() = 0;

    /*!
     * Unblock the neck yaw. [wait for reply]
     * \return true/false on success/failure.
     */
    virtual bool clearNeckYaw() = 0;

    /*!
     * Unblock the eyes. [wait for reply]
     * \return true/false on success/failure.
     */
    virtual bool clearEyes() = 0;

    /*!
     * Return the current tolerance defined by the user to gaze at
     * the target with the neck, meaning that the neck will turn to
     * the target with a final "misalignment" specified by this
     * tolerance; the residual rotation will be covered by the eyes
     * movement. By default, the tolerance is zero, that is the
     * neck will be perfectly aligned with the target. [wait for
     * reply]
     * \param angle the location where to store the current user
     *              tolerance [deg].
     * \return true/false on success/failure.
     */
    virtual bool getNeckAngleUserTolerance(double *angle) = 0;

    /*!
     * Specify a new tolerance to gaze at the target with the neck.
     *   [wait for reply]
     * \param angle the new angle user tolerance [deg].
     * \return true/false on success/failure.
     */
    virtual bool setNeckAngleUserTolerance(const double angle) = 0;

    /*!
     * Check once if the current trajectory is terminated. [wait for
     *   reply]
     * \param f where the result is returned.
     * \return true/false on success/failure.
     */
    virtual bool checkMotionDone(bool *f) = 0;

    /*!
     * Wait until the current trajectory is terminated. [wait for
     *   reply]
     * \param period specify the check time period (seconds).
     * \param timeout specify the check expiration time (seconds). If
     *         timeout<=0 (as by default) the check will be performed
     *         without time limitation.
     * \return true for success, false for failure and timeout
     *         expired.
     */
    virtual bool waitMotionDone(const double period=0.1, const double timeout=0.0) = 0;

    /*!
     * Check once if the saccade is terminated. [wait for reply]
     * \param f where the result is returned.
     * \return true/false on success/failure.
     */
    virtual bool checkSaccadeDone(bool *f) = 0;

    /*!
     * Wait until the current saccade is terminated. [wait for
     *   reply]
     * \param period specify the check time period (seconds).
     * \param timeout specify the check expiration time (seconds). If
     *         timeout<=0 (as by default) the check will be performed
     *         without time limitation.
     * \return true for success, false for failure and timeout
     *         expired.
     */
    virtual bool waitSaccadeDone(const double period=0.1, const double timeout=0.0) = 0;

    /*!
     * Ask for an immediate stop of the motion. [wait for reply]
     * \return true/false on success/failure.
     */
    virtual bool stopControl() = 0;

    /*!
     * Store the controller context. [wait for reply]
     * \param id specify where to store the returned context id.
     * \return true/false on success/failure.
     *
     * \note The context comprises the values of internal controller
     *       variables, such as the tracking mode, the trajectory
     *       time and so on.
     */
    virtual bool storeContext(int *id) = 0;

    /*!
     * Restore the controller context previously stored. [wait for
     *   reply]
     * \param id specify the context id to be restored.
     * \return true/false on success/failure.
     *
     * \note The context comprises the values of internal controller
     *       variables, such as the tracking mode, the trajectory
     *       time and so on.
     */
    virtual bool restoreContext(const int id) = 0;

    /*!
     * Delete a specified controller context. [wait for reply]
     * \param id specify the context id to be removed.
     * \return true/false on success/failure.
     */
    virtual bool deleteContext(const int id) = 0;

    /*!
     * Return useful info on the operating state of the controller.
     * [wait for reply]
     * \param info a property-like bottle containing the info.
     * \return true/false on success/failure.
     */
    virtual bool getInfo(yarp::os::Bottle &info) = 0;

    /*!
     * Register an event.
     * \param event the event to be registered.
     * \return true/false on success/failure.
     *
     * \note the special type "*" can be used to attach a callback to
     *       all the available events.
     */
    virtual bool registerEvent(yarp::dev::GazeEvent &event) = 0;

    /*!
     * Unregister an event.
     * \param event the event to be unregistered.
     * \return true/false on success/failure.
     */
    virtual bool unregisterEvent(yarp::dev::GazeEvent &event) = 0;

    /*!
     * Tweak low-level controller's parameters. [wait for reply]
     * \param options is a property-like bottle containing new values
     *                for the low-level controller's configuration.
     * \return true/false on success/failure.
     *
     * \note This method is intended for accessing low-level
     *       controller's configuration.
     */
    virtual bool tweakSet(const yarp::os::Bottle &options) = 0;

    /*!
     * Return low-level controller's parameters. [wait for reply]
     * \param options is a property-like bottle containing the
     *                current values of the low-level controller's
     *                configuration.
     * \return true/false on success/failure.
     *
     * \note This method is intended for accessing low-level
     *       controller's configuration.
     */
    virtual bool tweakGet(yarp::os::Bottle &options) = 0;
};

#endif // YARP_DEV_GAZECONTROL_H

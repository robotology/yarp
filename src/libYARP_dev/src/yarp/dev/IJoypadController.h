/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IJOYPADCONTROLLER_H
#define YARP_DEV_IJOYPADCONTROLLER_H

#include <yarp/sig/Vector.h>
#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>
#include <yarp/dev/TrackballData.h>
#include <yarp/dev/TouchData.h>
#include <yarp/dev/StickData.h>
#include <vector>
#include <string>

#define HAT_ACTIONS_ID_SHIFT 100

namespace yarp::dev {
class IJoypadController;
}

class YARP_dev_API yarp::dev::IJoypadController
{
public:
    enum class JoypadCtrl_coordinateMode {JypCtrlcoord_POLAR  =  0, JypCtrlcoord_CARTESIAN = 1};

public:

    /**
     * Destructor.
     */
    virtual ~IJoypadController();

    /**
     * @brief Get number of axes.
     *
     * @param axes_count unsigned int reference that will contain the result.
     * @return true if succeeded. false otherwise.
     */
    virtual yarp::dev::ReturnValue getAxisCount(size_t& axis_count) = 0;

    /**
     * @brief Get number of buttons.
     *
     * @param button_count unsigned int reference that will contain the result.
     * @return true if succeeded. false otherwise.
     */
    virtual yarp::dev::ReturnValue getButtonCount(size_t& button_count) = 0;

    /**
     * @brief Get number of trackballs.
     *
     * @param Trackball_count unsigned int reference that will contain the result.
     * @return true if succeeded. false otherwise.
     */
    virtual yarp::dev::ReturnValue getTrackballCount(size_t& Trackball_count) = 0;

    /**
     * @brief Get number of hats.
     *
     * @param Hat_count unsigned int reference that will contain the result.
     * @return true if succeeded. false otherwise.
     */
    virtual yarp::dev::ReturnValue getHatCount(size_t& Hat_count) = 0;

    /**
     * @brief Get the number of touch surface.
     *
     * Multiple touch surface can represent either multiple physical monotouch
     * surface or a multitouch surface or a combination of the two.
     *
     * @param touch_count unsigned int reference that will contain the result.
     * @return true if succeeded. false otherwise.
     */
    virtual yarp::dev::ReturnValue getTouchSurfaceCount(size_t& touch_count) = 0;

    /**
     * @brief Get the number of the sticks.
     *
     * @param stick_count unsigned int reference that will contain the result.
     * @return bool if succeeded, false otherwise.
     */
    virtual yarp::dev::ReturnValue getStickCount(size_t& stick_count) = 0;

    /**
     * @brief Get the Degree Of Freedom count for desired stick.
     *
     * @param stick_id Id of the stick. must be > -1 && < getStickCount(), return false otherwise.
     * @param DoF an unsigned int reference that will contain the value.
     * @return true if succeeded, false otherwise
     */
    virtual yarp::dev::ReturnValue getStickDoF(size_t stick_id, size_t& DoF) = 0;

    /**
     * @brief Get the value of a button.
     *
     * From 0-unpressed to 1-fullpressed and values in the middle in the
     * analog-face-button case.
     *
     * @param button_id Id of the button to get. Must be > -1 && < getButtonCount(), return false otherwise
     * @param value reference to be valued. The value will be from 0.0 (not even touched) to 1.0 (fully pressed)
     * @return true if succeeded, false otherwise
     */
    virtual yarp::dev::ReturnValue getButton(size_t button_id, double& value) = 0;

    /**
     * @brief Get the axes change of a Trackball.
     *
     * @param trackball_id Id of the Trackball to get. Must be > -1 && < getTrackballCount(), return false otherwise.
     * @param value reference to be valued. the value will be from 0.0 (not moving) to 1.0 (full velocity).
     * @return true if succeeded, false otherwise.
     */
    virtual yarp::dev::ReturnValue getTrackball(size_t trackball_id, yarp::dev::TrackballData& value) = 0;

    /**
     * @brief Get the value of an Hat.
     *
     * @param hat_id Id of the POV hat to get. Must be > -1 && < getHatCount(), return false otherwise.
     * @param value reference to be valued. use the YRPJOY_HAT_ macro series to get the currently pressed directions.
     * @return true if succeeded, false otherwise.
     */
    virtual yarp::dev::ReturnValue getHat(size_t hat_id, unsigned char& value) = 0;

    /**
     * @brief Get the value of an axis if present, return false otherwise.
     *
     * @param axis_id Id of the axis to get. must be > -1 && < getAxisCount(), return false otherwise.
     * @param value reference to be valued. the absolute boundaries for the values should be -1.0 and 1.0 However
     * the actual range depends on the physical device (example: analog trigger does not have a central position thus can
     * give a value from 0.0 to 0.1).
     * @return true if succeeded, false otherwise.
     */
    virtual yarp::dev::ReturnValue getAxis(size_t axis_id, double& value) = 0;

    /**
     * @brief Get the value of a stick if present, return false otherwise.
     *
     * @param stick_id Id of the stick to get. must be > -1 && < getStickCount(), return false otherwise.
     * @param value a vector that will contain the joystick position. the size of the vector will give you the Number of degrees of freedom
     * and the value will be from -1.0 to 1.0.
     * @param coordinate_mode to get data in cartesian mode or polar mode.
     * @return true if succeeded, false otherwise.
     */
    virtual yarp::dev::ReturnValue getStick(size_t stick_id, yarp::dev::StickData& value, JoypadCtrl_coordinateMode coordinate_mode) = 0;

    /**
     * @brief Get the value of a touch if present, return false otherwise.
     *
     * @param value a vector that will contain the touch values normalized from 0.0 to 1.0.
     * @return true if succeeded, false otherwise.
     */
    virtual yarp::dev::ReturnValue getTouch(size_t touch_id, std::vector<yarp::dev::TouchData>& value) = 0;
};

#define YRPJOY_HAT_CENTERED     0x00
#define YRPJOY_HAT_UP           0x01
#define YRPJOY_HAT_RIGHT        0x02
#define YRPJOY_HAT_DOWN         0x04
#define YRPJOY_HAT_LEFT         0x08
#define YRPJOY_HAT_RIGHTUP      (YRPJOY_HAT_RIGHT|YRPJOY_HAT_UP)
#define YRPJOY_HAT_RIGHTDOWN    (YRPJOY_HAT_RIGHT|YRPJOY_HAT_DOWN)
#define YRPJOY_HAT_LEFTUP       (YRPJOY_HAT_LEFT |YRPJOY_HAT_UP)
#define YRPJOY_HAT_LEFTDOWN     (YRPJOY_HAT_LEFT |YRPJOY_HAT_DOWN)

#endif //#define YARP_DEV_IJOYPADCONTROLLER_H

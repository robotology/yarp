/*
 * Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_IJOYPADCONTROLLER_H
#define YARP_DEV_IJOYPADCONTROLLER_H

#include <yarp/sig/Vector.h>


namespace yarp {
    namespace dev {
        class IJoypadController;
      }
}

class YARP_dev_API yarp::dev::IJoypadController
{

public:

    enum JoypadCtrl_result         {JypCtrlResult_ERROR = -1, JypCtrlResult_FALSE    = 0, JypCtrlResult_TRUE = 1};
    enum JoypadCtrl_coordinateMode {JypCtrlcoord_POLAR  =  0, JypCtrlcoord_CARTESIAN = 1};

    /**
    Destructor
    * @brief ~IJoypadController
    * @return
    */
    virtual ~IJoypadController(){}

    /**
      Get number of Axes
     * @brief getAxisCount
     * @param axes_count boolean reference that will contain the result
     * @return true if succeded. false otherwise
     */
    virtual bool getAxisCount(unsigned int& axes_count);
    /**
      Get number of Buttons
     * @brief getButtonCount
     * @param button_count boolean reference that will contain the result
     * @return true if succeded. false otherwise
     */
    virtual bool getButtonCount(unsigned int& button_count);

    /**
      tells if the device has some type of motion surface
     * @brief hasTouchSurface
     * @return JoypadCtrl_result::JoypadCtrl_TRUE if the device has any sort of Touch Surface, JoypadCtrl_result::JoypadCtrl_FALSE if not or JoypadCtrl_result::JoypadCtrl_ERROR if some error occurred
     */
    virtual JoypadCtrl_result hasTouchSurface();

    /**
      tells if the device has motion sensors
     * @brief hasMotionSensor
     * @return JoypadCtrl_result::JoypadCtrl_TRUE if the device has any sort of Touch Surface, JoypadCtrl_result::JoypadCtrl_FALSE if not or JoypadCtrl_result::JoypadCtrl_ERROR if some error occurred
     */
    virtual JoypadCtrl_result hasMotionSensor();

    /**
      get the number of the sticks
     * @brief getStickCount
     * @param stick_count reference that will contain the result
     * @return bool if succeded, false otherwise
     */
    virtual bool getStickCount(unsigned int& stick_count);

    /**
     Get the value of a button
    * @brief getButton
    * @param button_id Id of the button to get. Must be > -1 && < getAxesCount(), return false otherwise
    * @param value reference to be valued. true if pressed, false otherwise
    * @return true if succeded, false otherwise
    */
    virtual bool getButton(int button_id, float value);

    /**
     Get the value of an axis if present, return false otherwise
    * @brief getAxis
    * @param axis_id Id of the axis to get. must be > -1 && < getAxesCount(), return false otherwise
    * @param value reference to be valued
    * @return true if succeded, false otherwise
    */
    virtual bool getAxis(int axis_id, double value);

    /**
     Get the value of a stick if present, return false otherwise. Note: Every stick can be rapresented as a combination of two or more axes.. so their value can be retrieved as well
    * @brief getStick
    * @param stick_id Id of the stick to get. must be > -1 && < getStickCount(), return false otherwise
    * @param value a vector that will contain the joystick position. the size of the vector will give you the Number of degrees of freedom
    * @param coordinate_mode to get data in cartesian mode or polar (spheric in 3 dof position cases) mode
    * @return true if succeded, false otherwise
    */
    virtual bool getStick(int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode);

    /**
     Get the value of a touch if present, return false otherwise
    * @brief getTouch
    * @param value a vector that will contain the joystick position. the size of the vector will give you the Number of degrees of freedom
    * @return true if succeded, false otherwise
    */
    virtual bool getTouch(yarp::sig::Vector& value);
}

/*
 * Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_IJOYPADCONTROLLER_H
#define YARP_DEV_IJOYPADCONTROLLER_H

#include <yarp/sig/Vector.h>
#include <yarp/dev/api.h>
#include <yarp/os/Vocab.h>

namespace yarp
{
    namespace dev
    {
        class IJoypadController;
    }
}

class  YARP_dev_API yarp::dev::IJoypadController
{

public:

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
     * @param axes_count unsigned int reference that will contain the result
     * @return true if succeded. false otherwise
     */
    virtual bool getAxisCount(unsigned int& axis_count) = 0;

    /**
      Get number of Buttons
     * @brief getButtonCount
     * @param button_count unsigned int reference that will contain the result
     * @return true if succeded. false otherwise
     */
    virtual bool getButtonCount(unsigned int& button_count) = 0;

    /**
      Get number of trackballs
     * @brief getTrackballCount
     * @param Trackball_count unsigned int reference that will contain the result
     * @return true if succeded. false otherwise
     */
    virtual bool getTrackballCount(unsigned int& Trackball_count) = 0;

    /**
      Get number of Hats
     * @brief getHatsCount
     * @param Hat_count unsigned int reference that will contain the result
     * @return true if succeded. false otherwise
     */
    virtual bool getHatCount(unsigned int& Hat_count) = 0;

    /**
      get the number of touch surface
     * @brief hasTouchSurface
     * @param touch_count unsigned int reference that will contain the result
     * @return true if succeded. false otherwise
     */
    virtual bool getTouchSurfaceCount(unsigned int& touch_count) = 0;

    /**
      get the number of the sticks
     * @brief getStickCount
     * @param stick_count unsigned int reference that will contain the result
     * @return bool if succeded, false otherwise
     */
    virtual bool getStickCount(unsigned int& stick_count) = 0;

    /**
     Get the Degree Of Freedom count for desired stick
    * @brief getStickDoF
    * @param stick_id Id of the stick. must be > -1 && < getStickCount(), return false otherwise
    * @param DoF an unsigned int reference that will contain the value.
    * @return true if succeded, false otherwise
    */
    virtual bool getStickDoF(unsigned int stick_id, unsigned int& DoF) = 0;

    /**
     Get the value of a button. from 0-unpressed to 1-fullpressed and values in the middle in the analog-face-button case
    * @brief getButton
    * @param button_id Id of the button to get. Must be > -1 && < getButtonCount(), return false otherwise
    * @param value reference to be valued. true if pressed, false otherwise
    * @return true if succeded, false otherwise
    */
    virtual bool getButton(unsigned int button_id, float& value) = 0;

    /**
     Get the axes change of a Trackball.
    * @brief getTrackball
    * @param trackball_id Id of the Trackball to get. Must be > -1 && < getTrackballCount(), return false otherwise
    * @param value reference to be valued. true if pressed, false otherwise
    * @return true if succeded, false otherwise
    */
    virtual bool getTrackball(unsigned int trackball_id, yarp::sig::Vector& value) = 0;

    /**
     Get the value of an Hat.
    * @brief getHat
    * @param hat_id Id of the POV hat to get. Must be > -1 && < getHatCount(), return false otherwise
    * @param value reference to be valued.
    * @return true if succeded, false otherwise
    */
    virtual bool getHat(unsigned int hat_id, unsigned char& value) = 0;

    /**
     Get the value of an axis if present, return false otherwise
    * @brief getAxis
    * @param axis_id Id of the axis to get. must be > -1 && < getAxisCount(), return false otherwise
    * @param value reference to be valued
    * @return true if succeded, false otherwise
    */
    virtual bool getAxis(unsigned int axis_id, double& value) = 0;

    /**
     Get the value of a stick if present, return false otherwise.
    * @brief getStick
    * @param stick_id Id of the stick to get. must be > -1 && < getStickCount(), return false otherwise
    * @param value a vector that will contain the joystick position. the size of the vector will give you the Number of degrees of freedom
    * @param coordinate_mode to get data in cartesian mode or polar (spheric in 3 dof position cases) mode
    * @return true if succeded, false otherwise
    */
    virtual bool getStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode) = 0;

    /**
     Get the value of a touch if present, return false otherwise
    * @brief getTouch
    * @param value a vector that will contain the joystick position. the size of the vector will give you the Number of degrees of freedom
    * @return true if succeded, false otherwise
    */
    virtual bool getTouch(unsigned int touch_id, yarp::sig::Vector& value) = 0;
};
#define YRPJOY_HAT_CENTERED	 0x00
#define YRPJOY_HAT_UP		 0x01
#define YRPJOY_HAT_RIGHT	 0x02
#define YRPJOY_HAT_DOWN		 0x04
#define YRPJOY_HAT_LEFT		 0x08
#define YRPJOY_HAT_RIGHTUP	 (YRPJOY_HAT_RIGHT|YRPJOY_HAT_UP)
#define YRPJOY_HAT_RIGHTDOWN (YRPJOY_HAT_RIGHT|YRPJOY_HAT_DOWN)
#define YRPJOY_HAT_LEFTUP	 (YRPJOY_HAT_LEFT |YRPJOY_HAT_UP)
#define YRPJOY_HAT_LEFTDOWN	 (YRPJOY_HAT_LEFT |YRPJOY_HAT_DOWN)

#define VOCAB_OK          VOCAB2('o','k')
#define VOCAB_FAILED      VOCAB4('f','a','i','l')
#define VOCAB_IJOYPADCTRL VOCAB4('i','j','p','c')
#define VOCAB_GET         VOCAB4('j','g','e','t')
#define VOCAB_BUTTON      VOCAB4('j','b','t','n')
#define VOCAB_TRACKBALL   VOCAB4('j','t','r','b')
#define VOCAB_HAT         VOCAB4('j','h','a','t')
#define VOCAB_AXIS        VOCAB4('j','a','x','s')
#define VOCAB_STICK       VOCAB4('j','s','t','k')
#define VOCAB_STICKDOF    VOCAB4('j','d','o','f')
#define VOCAB_TOUCH       VOCAB4('j','t','c','h')
#define VOCAB_COUNT       VOCAB4('j','c','n','t')
#define VOCAB_VALUE       VOCAB4('j','v','a','l')
#define VOCAB_POLAR       VOCAB4('j','p','l','r')
#define VOCAB_CARTESIAN   VOCAB4('j','c','r','t')

#endif //#define YARP_DEV_IJOYPADCONTROLLER_H

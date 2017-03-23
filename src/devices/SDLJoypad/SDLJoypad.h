/*
 * Copyright (C) 2017 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef SDLJOYPAD_H
#define SDLJOYPAD_H

#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/DeviceDriver.h>
#include <SDL.h>
#include <vector>


namespace yarp {
    namespace dev {
        class SDLJoypad;
      }
}

class yarp::dev::SDLJoypad : public yarp::dev::IJoypadController,
                             public yarp::dev::DeviceDriver
{

    std::vector<SDL_Joystick*> m_device;
    unsigned int               m_buttonCount;
    unsigned int               m_axisCount;
    unsigned int               m_stickCount;
    unsigned int               m_touchCount;
    unsigned int               m_ballCount;
    unsigned int               m_hatCount;
    bool                       m_allJoypad;

    void updateJoypad();
public:

    SDLJoypad();
    ~SDLJoypad();
    //DeviceDriver
    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    //IJoypadController
    virtual bool getAxisCount(unsigned int& axis_count) YARP_OVERRIDE;

    virtual bool getButtonCount(unsigned int& button_count) YARP_OVERRIDE;

    virtual bool getHatCount(unsigned int& hat_count) YARP_OVERRIDE;

    virtual bool getTrackballCount(unsigned int& trackball_count) YARP_OVERRIDE;

    virtual bool getTouchSurfaceCount(unsigned int& touch_count) YARP_OVERRIDE;

    virtual bool getStickCount(unsigned int& stick_count) YARP_OVERRIDE;

    virtual bool getStickDoF(unsigned int  stick_id, unsigned int& DoF) YARP_OVERRIDE;

    virtual bool getButton(unsigned int  button_id, float& value) YARP_OVERRIDE;

    virtual bool getTrackball(unsigned int  trackball_id, yarp::sig::Vector& value) YARP_OVERRIDE;

    virtual bool getHat(unsigned int  hat_id, unsigned char& value) YARP_OVERRIDE;

    virtual bool getAxis(unsigned int  axis_id, double& value) YARP_OVERRIDE;

    virtual bool getStick(unsigned int  stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode) YARP_OVERRIDE;

    virtual bool getTouch(unsigned int  touch_id, yarp::sig::Vector& value) YARP_OVERRIDE;
};


#endif //#define SDLJOYPAD_H

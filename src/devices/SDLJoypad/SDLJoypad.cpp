/*
 * Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "SDLJoypad.h"
#include <stdio.h>
#include <yarp/os/LogStream.h>


using namespace yarp::dev;
using namespace std;
SDLJoypad::SDLJoypad()
{
    m_buttonCount = 0;
    m_axisCount   = 0;
    m_stickCount  = 0;
    m_touchCount  = 0;
    m_ballCount   = 0;
    m_hatCount    = 0;
    m_device      = 0;
}

SDLJoypad::~SDLJoypad()
{

}

bool SDLJoypad::open(yarp::os::Searchable& rf)
{
    int joy_id, joystick_num;
    if (SDL_InitSubSystem( SDL_INIT_JOYSTICK ) < 0 )
    {
        yError ( "SDLJoypad: Unable to initialize Joystick: %s\n", SDL_GetError() );
        return false;
    }

    joy_id       = 0;
    joystick_num = SDL_NumJoysticks();
    if (joystick_num == 0)
    {
        yError ( "SDLJoypad: No joysticks found\n");
        return false;
    }
    else if (joystick_num == 1)
    {
        joy_id = 0;
        yInfo ( "SDLJoypad: One joystick found \n");
        yInfo ( "SDLJoypad: Using joystick: %s \n", SDL_JoystickName(joy_id));
    }
    else
    {
        yInfo ( "SDLJoypad: More than one joystick found:\n");
        for (int i = 0; i < joystick_num; i++)
        {
            yInfo ( "%d: %s\n", i, SDL_JoystickName(i));
        }
        yInfo ( "\n");

        // choose between multiple joysticks
        if (rf.check("DefaultJoystickNumber"))
        {
            joy_id = rf.find("DefaultJoystickNumber").asInt();
            yInfo ( "SDLJoypad: Multiple joysticks found, using #%d, as specified in the configuration options\n", joy_id);
        }
        else
        {
            yWarning ( "SDLJoypad: No default joystick specified in the configuration options\n");
            yWarning ( "SDLJoypad: Which joystick you want to use? (choose number) \n");
            cin >> joy_id;
        }
    }

    m_device = SDL_JoystickOpen ( joy_id );
    if ( m_device == NULL )
    {
        yError ( "SDLJoypad: Could not open joystick\n" );
        return false;
    }

    // Obtaining Joystick capabilities
    m_axisCount   = SDL_JoystickNumAxes(m_device);
    m_ballCount   = SDL_JoystickNumBalls(m_device);
    m_hatCount    = SDL_JoystickNumHats(m_device);
    m_buttonCount = SDL_JoystickNumButtons(m_device);

    return true;
}

bool SDLJoypad::close()
{
    return false;
}

bool SDLJoypad::getAxisCount(unsigned int& axes_count)
{
    axes_count = m_axisCount;
    return true;
}

bool SDLJoypad::getButtonCount(unsigned int& button_count)
{
    button_count = m_buttonCount;
    return true;
}

bool SDLJoypad::getTrackballCount(unsigned int& trackball_count)
{
    trackball_count = m_ballCount;
    return true;
}

bool SDLJoypad::getHatCount(unsigned int& hat_count)
{
    hat_count = m_hatCount;
    return true;
}

bool SDLJoypad::getTouchSurfaceCount(unsigned int& touch_count)
{
    touch_count = m_touchCount;
    return true;
}

bool SDLJoypad::getStickCount(unsigned int& stick_count)
{
    stick_count = m_stickCount;
    return true;
}

bool SDLJoypad::getStickDoF(unsigned int stick_id, unsigned int& DoF)
{
    return false;
}

bool SDLJoypad::getButton(unsigned int button_id, float& value)
{
    updateJoypad();
    value = float(SDL_JoystickGetButton(m_device, button_id));
    return true;
}

bool SDLJoypad::getAxis(unsigned int axis_id, double& value)
{
    updateJoypad();
    value = 2 * ((float(SDL_JoystickGetAxis(m_device, axis_id)) - (-32.768)) / 0xffff);
    return true;
}

bool SDLJoypad::getStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode)
{
    return false;
}

bool SDLJoypad::getTouch(unsigned int touch_id, yarp::sig::Vector& value)
{
    return false;
}

bool SDLJoypad::getHat(unsigned int hat_id, unsigned char& value)
{
    updateJoypad();
    value = SDL_JoystickGetHat(m_device, hat_id);//TODO: map from their HAT define to our in case of #define changes
    return true;
}

bool SDLJoypad::getTrackball(unsigned int trackball_id, yarp::sig::Vector& value)
{
    updateJoypad();
    int x,y;
    if(SDL_JoystickGetBall(m_device, trackball_id, &x, &y) == -1)
    {
        yError() << "SDLJoypad: SDL_JoystickGetBall returned error";
        return false;
    }
    value.resize(2);
    value[0] = x;
    value[1] = y;
    return true;
}

void SDLJoypad::updateJoypad()
{
    SDL_JoystickUpdate();
}

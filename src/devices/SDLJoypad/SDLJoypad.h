/*
 * Copyright (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * Authors: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef SDLJOYPAD_H
#define SDLJOYPAD_H

#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/DeviceDriver.h>
#include <SDL.h>
#include <vector>
#include <yarp/os/RateThread.h>


namespace yarp {
    namespace dev {
        class  SDLJoypad;
        namespace SDLJoypadImpl
        {
            struct stick;
        }
      }
}

struct yarp::dev::SDLJoypadImpl::stick
{
    std::vector<unsigned int>  axes_ids;
    float                      deadZone;
    std::vector<int>           direction;
};

class yarp::dev::SDLJoypad : public yarp::dev::IJoypadEventDriven,//public yarp::dev::IJoypadController,
                             public yarp::dev::DeviceDriver
{
    typedef std::vector<yarp::dev::SDLJoypadImpl::stick> stickVector;

    SDL_Event                  m_event;
    std::vector<SDL_Joystick*> m_device;
    unsigned int               m_buttonCount;
    unsigned int               m_axisCount;
    unsigned int               m_stickCount;
    unsigned int               m_touchCount;
    unsigned int               m_ballCount;
    unsigned int               m_hatCount;
    bool                       m_allJoypad;
    stickVector                m_sticks;
    std::vector<bool>          m_axes;
    double                     m_actionTimestamp;

    void updateJoypad();
    void pollActions();
    bool parseStickInfo(const yarp::os::Searchable& cfg);
    bool getPureAxis(unsigned int axis_id, double& value);
public:

    SDLJoypad();
    virtual ~SDLJoypad();
    //DeviceDriver
    virtual bool open(yarp::os::Searchable& config) override;
    virtual bool close() override;

    //IJoypadController
    virtual bool getRawAxisCount(unsigned int& axis_count) override;

    virtual bool getRawButtonCount(unsigned int& button_count) override;

    virtual bool getRawHatCount(unsigned int& hat_count) override;

    virtual bool getRawTrackballCount(unsigned int& trackball_count) override;

    virtual bool getRawTouchSurfaceCount(unsigned int& touch_count) override;

    virtual bool getRawStickCount(unsigned int& stick_count) override;

    virtual bool getRawStickDoF(unsigned int  stick_id, unsigned int& DoF) override;

    virtual bool getRawButton(unsigned int  button_id, float& value) override;

    virtual bool getRawTrackball(unsigned int  trackball_id, yarp::sig::Vector& value) override;

    virtual bool getRawHat(unsigned int  hat_id, unsigned char& value) override;

    virtual bool getRawAxis(unsigned int  axis_id, double& value) override;

    virtual bool getRawStick(unsigned int  stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode) override;

    virtual bool getRawTouch(unsigned int  touch_id, yarp::sig::Vector& value) override;
};


#endif //#define SDLJOYPAD_H

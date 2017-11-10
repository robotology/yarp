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

/**
* @ingroup dev_impl_wrapper
*
* \section SDLJoypad Description of input parameters
* \brief Device that reads inputs of Joypads compatible with the SDL library.
*
* Parameters accepted in the config argument of the open method:
* |   Parameter name      | Type   | Units | Default Value | Required                                    | Description                               | Notes                                                                                                                                   |
* |:---------------------:|:------:|:-----:|:-------------:|:-------------------------------------------:|:-----------------------------------------:|:---------------------------------------------------------------------------------------------------------------------------------------:|
* | UseAllJoypadAsOne     | string |       |               | if there are more than one joypad connected | merge together multiple joypad            | set it to 1 to have all the connected joypad as one                                                                                     |
* | DefaultJoystickNumber | string |       | will be asked | No                                          | select the id of the joypad to use        | important if there are more than one joypad and UseAllJoypadAsOne is setted to 0. if not present, a input will be requested to the user |
* | sticks                | int    |       |               | Yes                                         | count of sticks to configure              | for each stick there must be a group called STICK<id here> with data on the axis to be configured as stickl                             |
* | STICK[ID]             | group  |       |               | depending on 'sticks' parameter             | group containing axes data for this stick |                                                                                                                                         |
* | axes                  | int    |       |               | yes                                         | axes count for this stick                 | set it for each stick in the proper stick group                                                                                         |
* | axis[ID]_id           | int    |       |               | depending on axes parameter                 | axis id for current stick related axis    | set it for each stick in the proper stick group                                                                                         |
* | invert_axis_[ID]      | bool   |       | false         | no                                          | invert the current axis                   | set it for each stick in the proper stick group                                                                                         |
* | deadZone              | double |       | 0.0           | yes                                         | set the deadzone for this stick           | set it for each stick in the proper stick group                                                                                         |
**/

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

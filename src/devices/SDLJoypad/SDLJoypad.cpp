/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "SDLJoypad.h"
#include <cstdio>
#include <yarp/os/LogStream.h>
#include <cmath>
#include <yarp/os/Time.h>
#include <iostream>


using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;
using namespace std;
using namespace SDLJoypadImpl;

namespace {
YARP_LOG_COMPONENT(SDLJOYPAD, "yarp.device.SDLJoypad")
constexpr double actionsExecutionTime = 1.0;
}



SDLJoypad::SDLJoypad() = default;

SDLJoypad::~SDLJoypad() = default;

bool SDLJoypad::open(yarp::os::Searchable& rf)
{
    if(rf.check("help"))
    {
        yCInfo(SDLJOYPAD) << "parameters:";
        yCInfo(SDLJOYPAD);
        yCInfo(SDLJOYPAD) << "UseAllJoypadAsOne     - set it to 1 to have all the connected joypad as one";
        yCInfo(SDLJOYPAD) << "DefaultJoystickNumber - select the id of the joypad to use if there are more than one joypad and UseAllJoypadAsOne is set to 0";
        yCInfo(SDLJOYPAD) << "stick                 - the number of stick to configure. a stick is simply a wrapping of 2 or more axes so for every stick";
        yCInfo(SDLJOYPAD) << "                        a group named STICK*ID* containing the stick's parameters is searched. ";
        yCInfo(SDLJOYPAD);
        yCInfo(SDLJOYPAD) << "stick groups parameters:";
        yCInfo(SDLJOYPAD) << "axes             - axes count for this stick";
        yCInfo(SDLJOYPAD) << "axis[ID]_id      - axis id for current stick related axis";
        yCInfo(SDLJOYPAD) << "invert_axis_[ID] - invert the current axis";
        yCInfo(SDLJOYPAD) << "deadZone         - set the deadzone for this stick";
        return false;
    }

    int    joy_id;
    size_t joystick_num;
    int    actionCount;

    if (SDL_InitSubSystem( SDL_INIT_JOYSTICK ) < 0 )
    {
        yCError(SDLJOYPAD, "Unable to initialize Joystick: %s", SDL_GetError());
        return false;
    }

    joy_id       = 0;
    joystick_num = SDL_NumJoysticks();
    actionCount  = 0;

    if (joystick_num == 0)
    {
        yCError(SDLJOYPAD, "No joysticks found");
        return false;
    }
    else if (joystick_num == 1)
    {
        joy_id = 0;
        yCInfo(SDLJOYPAD, "One joystick found");
        yCInfo(SDLJOYPAD, "Using joystick: %s", SDL_JoystickName(joy_id));
    }
    else
    {
        yCInfo(SDLJOYPAD, "More than one joystick found:");
        for (size_t i = 0; i < joystick_num; i++)
        {
            yCInfo(SDLJOYPAD) << i << ":" << SDL_JoystickName(i);
        }
        yCInfo(SDLJOYPAD);
        if(!rf.check("UseAllJoypadAsOne"))
        {
            if(rf.find("UseAllJoypadAsOne").asBool())
            {
                // choose between multiple joysticks
                if (rf.check("DefaultJoystickNumber"))
                {
                    joy_id = rf.find("DefaultJoystickNumber").asInt32();
                    yCInfo(SDLJOYPAD, "Multiple joysticks found, using #%d, as specified in the configuration options", joy_id);
                }
                else
                {
                    yCWarning(SDLJOYPAD, "No default joystick specified in the configuration options");
                    yCWarning(SDLJOYPAD, "Which joystick you want to use? (choose number)");
                    cin >> joy_id;
                }
                m_allJoypad = false;
            }
            else
            {
                m_allJoypad = true;
            }
        }
        else
        {
            yCError(SDLJOYPAD) << "Missing UseAllJoypadAsOne parameter";
            return false;
        }
    }

    if(m_allJoypad)
    {
        for(size_t i = 0; i < joystick_num; ++i)
        {
            m_device.push_back(SDL_JoystickOpen(i));
        }
    }
    else
    {
        m_device.push_back(SDL_JoystickOpen(joy_id));
    }

    for(size_t i = 0; i < m_device.size(); ++i)
    {
        if ( m_device[i] == nullptr )
        {
            yCError(SDLJOYPAD) << "Could not open joystick with id" << i;
            return false;
        }

        // Obtaining Joystick capabilities
        m_axisCount   += SDL_JoystickNumAxes(m_device[i]);
        m_ballCount   += SDL_JoystickNumBalls(m_device[i]);
        m_hatCount    += SDL_JoystickNumHats(m_device[i]);
        m_buttonCount += SDL_JoystickNumButtons(m_device[i]);
    }

    if(parseActions(rf, &actionCount))
    {
        if(actionCount)
        {
            if(SDL_JoystickEventState(SDL_ENABLE) < 0)
            {
                yCError(SDLJOYPAD) << "SDLJoypad:" << SDL_GetError();
                return false;
            }
            yCInfo(SDLJOYPAD) << "Actions successfully parsed and linked to the joypad";
        }
    }
    else
    {
        yCError(SDLJOYPAD) << "Error while parsing actions";
        return false;
    }

    if(!parseStickInfo(rf)){return false;}
    return true;
}

bool SDLJoypad::parseStickInfo(const yarp::os::Searchable& cfg)
{
    if(!cfg.check("sticks") || !cfg.find("sticks").isInt32())
    {
        yCError(SDLJOYPAD) << "Missing 'sticks' parameter or not an integer";
        return false;
    }

    for(unsigned int i = 0; i < m_axisCount; i++)
    {
        m_axes.push_back(true);
    }

    m_stickCount = cfg.find("sticks").asInt32();
    for(unsigned int i = 0; i < m_stickCount; i++)
    {
        string stickName;
        int    axesCount;
        stick  currentStick;

        stickName = "STICK"+std::to_string(i);

        if(!cfg.check(stickName))
        {
            yCError(SDLJOYPAD) << "Missing" << stickName << "group in configuration";
            return false;
        }

        Bottle& stickParams = cfg.findGroup(stickName);

        if(0 == stickParams.size())
        {
            yCError(SDLJOYPAD) << "Group" << stickName << "is empty";
            return false;
        }

        if(!stickParams.check("axes") || !stickParams.find("axes").isInt32())
        {
            yCError(SDLJOYPAD) << "Missing 'axes' count in" << stickName << "group or not an integer";
            return false;
        }

        axesCount = stickParams.find("axes").asInt32();

        for(int j = 0; j < axesCount; j++)
        {
            string       axisName, invertName;
            unsigned int axis_id;
            axisName   = "axis"         + std::to_string(j) + "_id";
            invertName = "invert_axis_" + std::to_string(j);

            if(!stickParams.check(axisName) || !stickParams.find(axisName).isInt32())
            {
                yCError(SDLJOYPAD) << "Missing" << axisName << "param in" << stickName << "group or not an integer.";
                return false;
            }

            axis_id = (unsigned int)stickParams.find(axisName).asInt32();
            if(axis_id > m_axisCount - 1)
            {
                yCError(SDLJOYPAD) << "Axis id out of bound";
                return false;
            }

            if(!stickParams.check(invertName) || !stickParams.find(invertName).isBool())
            {
                yCError(SDLJOYPAD) << "Missing" << invertName << "param in" << stickName << "group or not an bool.";
                return false;
            }

            currentStick.axes_ids.push_back(axis_id);
            currentStick.direction.push_back(stickParams.find(invertName).asBool() ? -1 : 1);
            m_axes[axis_id] = false;
        }

        if(!stickParams.check("deadZone") || !stickParams.find("deadZone").isFloat64())
        {
            yCError(SDLJOYPAD) << "Missing deadZone param in" << stickName << "group or not an double.";
            return false;
        }

        currentStick.deadZone = stickParams.find("deadZone").asFloat64();
        m_sticks.push_back(currentStick);
    }
    return true;
}

bool SDLJoypad::close()
{
    return false;
}

bool SDLJoypad::getRawAxisCount(unsigned int& axes_count)
{
    axes_count = m_axisCount;
    return true;
}

bool SDLJoypad::getRawButtonCount(unsigned int& button_count)
{
    button_count = m_buttonCount;
    return true;
}

bool SDLJoypad::getRawTrackballCount(unsigned int& trackball_count)
{
    trackball_count = m_ballCount;
    return true;
}

bool SDLJoypad::getRawHatCount(unsigned int& hat_count)
{
    hat_count = m_hatCount;
    return true;
}

bool SDLJoypad::getRawTouchSurfaceCount(unsigned int& touch_count)
{
    touch_count = m_touchCount;
    return true;
}

bool SDLJoypad::getRawStickCount(unsigned int& stick_count)
{
    stick_count = m_stickCount;
    return true;
}

bool SDLJoypad::getRawStickDoF(unsigned int stick_id, unsigned int& DoF)
{
    if(stick_id > m_sticks.size()-1)
    {
        yCError(SDLJOYPAD) << "SDL_Joypad: stick_id out of bounds when calling 'getStickDoF'' method";
        return false;
    }
    DoF = 2;
    return true;
}

bool SDLJoypad::getRawButton(unsigned int button_id, float& value)
{
    if(button_id > m_buttonCount - 1){yCError(SDLJOYPAD) << "Button id out of bound!"; return false;}
    updateJoypad();
    size_t i;
    for(i = 0; i < m_device.size(); ++i)
    {
        unsigned int localCount = SDL_JoystickNumButtons(m_device[i]);
        if(button_id > localCount - 1)
        {
            button_id -= localCount;
        }
        else
        {
            break;
        }
    }
    value = float(SDL_JoystickGetButton(m_device[i], button_id));
    if(value > 0.5 && m_actions.find(button_id) != m_actions.end() && yarp::os::Time::now() - m_actionTimestamp > actionsExecutionTime)
    {
        executeAction(button_id);
        m_actionTimestamp = yarp::os::Time::now();
    }
    return true;
}

bool SDLJoypad::getPureAxis(unsigned int axis_id, double& value)
{
    if(axis_id > m_axisCount - 1){yCError(SDLJOYPAD) << "Axis id out of bound!"; return false;}
    size_t i;

    for(i = 0; i < m_device.size(); ++i)
    {
        unsigned int localCount;
        localCount = SDL_JoystickNumAxes(m_device[i]);
        if(axis_id > localCount - 1)
        {
            axis_id -= localCount;
        }
        else
        {
            break;
        }
    }

    value = 2 * ((float(SDL_JoystickGetAxis(m_device[i], axis_id)) - (-32.768)) / 0xffff);
    return true;
}

bool SDLJoypad::getRawAxis(unsigned int axis_id, double& value)
{
    if(axis_id > m_axisCount - 1){yCError(SDLJOYPAD) << "Axis id out of bound!"; return false;}
    //if(!m_axes.at(axis_id)) {yCWarning(SDLJOYPAD) << "Requested axis is part of a stick!";}
    updateJoypad();
    return getPureAxis(axis_id, value);
}

yarp::sig::Vector Vector3(const double& x, const double& y)
{
    Vector ret;
    ret.push_back(x);
    ret.push_back(y);
    return ret;
}

bool SDLJoypad::getRawStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode)
{
    if (stick_id > m_stickCount - 1){yCError(SDLJOYPAD) << "Stick id out of bound!"; return false;}
    value.clear();
    updateJoypad();
    stick& stk = m_sticks[stick_id];
    double val;

    for(size_t i = 0; i < stk.axes_ids.size(); i++)
    {
        if(!getRawAxis(stk.axes_ids[i], val)) {return false;}
        value.push_back(val * stk.direction[i] * (fabs(val) > stk.deadZone));
    }

    if (coordinate_mode == JypCtrlcoord_POLAR)
    {
        if (stk.axes_ids.size() > 2)
        {
            yCError(SDLJOYPAD) << "Polar coordinate system is supported only for bidimensional stick at the moment";
            return false;
        }
        value = Vector3(sqrt(value[0] * value[0] + value[1] * value[1]), atan2(value[0], value[1]));
    }
    return true;
}

bool SDLJoypad::getRawTouch(unsigned int touch_id, yarp::sig::Vector& value)
{
    return false;
}

bool SDLJoypad::getRawHat(unsigned int hat_id, unsigned char& value)
{
    if(hat_id > m_hatCount - 1){yCError(SDLJOYPAD) << "Axis id out of bound!"; return false;}
    updateJoypad();
    size_t i;
    for(i = 0; i < m_device.size(); ++i)
    {
        unsigned int localCount = SDL_JoystickNumHats(m_device[i]);
        if(hat_id > localCount - 1)
        {
            hat_id -= localCount;
        }
        else
        {
            break;
        }
    }

    value = SDL_JoystickGetHat(m_device[i], hat_id);//TODO: map from their HAT define to our in case of #define changes
    return true;
}

bool SDLJoypad::getRawTrackball(unsigned int trackball_id, yarp::sig::Vector& value)
{
    if(trackball_id > m_ballCount - 1){yCError(SDLJOYPAD) << "Trackball id out of bound!"; return false;}
    updateJoypad();
    int x, y;
    size_t i;
    for(i = 0; i < m_device.size(); ++i)
    {
        unsigned int localCount = SDL_JoystickNumBalls(m_device[i]);
        if(trackball_id > localCount - 1)
        {
            trackball_id -= localCount;
        }
        else
        {
            break;
        }
    }
    if(SDL_JoystickGetBall(m_device[i], trackball_id, &x, &y) == -1)
    {
        yCError(SDLJOYPAD) << "SDL_JoystickGetBall returned error";
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

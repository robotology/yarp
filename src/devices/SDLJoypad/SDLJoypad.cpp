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
using namespace SDLJoypadImpl;

namespace {
YARP_LOG_COMPONENT(SDLJOYPAD, "yarp.device.SDLJoypad")
constexpr double actionsExecutionTime = 1.0;
}



SDLJoypad::SDLJoypad() = default;

SDLJoypad::~SDLJoypad() = default;

bool SDLJoypad::open(yarp::os::Searchable& rf)
{
/*  if(rf.check("help"))
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
*/

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
                    std::cin >> joy_id;
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

    if(m_actionsHandler.parseActions(rf, m_buttonCount, &actionCount))
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

    for(size_t i = 0; i < m_axisCount; i++)
    {
        m_axes.push_back(true);
    }

    m_stickCount = cfg.find("sticks").asInt32();
    for(size_t i = 0; i < m_stickCount; i++)
    {
        std::string stickName;
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
            std::string       axisName, invertName;
            size_t axis_id;
            axisName   = "axis"         + std::to_string(j) + "_id";
            invertName = "invert_axis_" + std::to_string(j);

            if(!stickParams.check(axisName) || !stickParams.find(axisName).isInt32())
            {
                yCError(SDLJOYPAD) << "Missing" << axisName << "param in" << stickName << "group or not an integer.";
                return false;
            }

            axis_id = (size_t)stickParams.find(axisName).asInt32();
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

ReturnValue SDLJoypad::getAxisCount(size_t& axes_count)
{
    axes_count = m_axisCount;
    return ReturnValue_ok;
}

ReturnValue SDLJoypad::getButtonCount(size_t& button_count)
{
    button_count = m_buttonCount;
    return ReturnValue_ok;
}

ReturnValue SDLJoypad::getTrackballCount(size_t& trackball_count)
{
    trackball_count = m_ballCount;
    return ReturnValue_ok;
}

ReturnValue SDLJoypad::getHatCount(size_t& hat_count)
{
    hat_count = m_hatCount;
    return ReturnValue_ok;
}

ReturnValue SDLJoypad::getTouchSurfaceCount(size_t& touch_count)
{
    touch_count = m_touchCount;
    return ReturnValue_ok;
}

ReturnValue SDLJoypad::getStickCount(size_t& stick_count)
{
    stick_count = m_stickCount;
    return ReturnValue_ok;
}

ReturnValue SDLJoypad::getStickDoF(size_t stick_id, size_t& DoF)
{
    if(stick_id > m_sticks.size()-1)
    {
        yCError(SDLJOYPAD) << "SDL_Joypad: stick_id out of bounds when calling 'getStickDoF'' method";
        return ReturnValue_error_input_out_of_bounds;
    }
    DoF = 2;
    return ReturnValue_ok;
}

ReturnValue SDLJoypad::getButton(size_t button_id, double& value)
{
    if(button_id > m_buttonCount - 1)
    {
        yCError(SDLJOYPAD) << "Button id out of bound!";
        return ReturnValue_error_input_out_of_bounds;
    }
    updateJoypad();
    size_t i;
    for(i = 0; i < m_device.size(); ++i)
    {
        size_t localCount = SDL_JoystickNumButtons(m_device[i]);
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
    if(value > 0.5 && m_actionsHandler.actionDetected(button_id) &&
       yarp::os::Time::now() - m_actionTimestamp > actionsExecutionTime)
    {
        m_actionsHandler.executeAction(button_id);
        m_actionTimestamp = yarp::os::Time::now();
    }
    return ReturnValue_ok;
}

ReturnValue SDLJoypad::getPureAxis(size_t axis_id, double& value)
{
    if(axis_id > m_axisCount - 1)
    {
        yCError(SDLJOYPAD) << "Axis id out of bound!";
        return ReturnValue_error_input_out_of_bounds;
    }
    size_t i;

    for(i = 0; i < m_device.size(); ++i)
    {
        size_t localCount;
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
    return ReturnValue_ok;
}

ReturnValue SDLJoypad::getAxis(size_t axis_id, double& value)
{
    if(axis_id > m_axisCount - 1)
    {
        yCError(SDLJOYPAD) << "Axis id out of bound!";
        return ReturnValue_error_input_out_of_bounds;
    }
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

ReturnValue SDLJoypad::getStick(size_t  stick_id, yarp::dev::StickData& value, JoypadCtrl_coordinateMode coordinate_mode)
{
    if (stick_id > m_stickCount - 1)
    {
        yCError(SDLJOYPAD) << "Stick id out of bound!";
        return ReturnValue_error_input_out_of_bounds;
    }
    updateJoypad();
    stick& stk_config = m_sticks[stick_id];
    double val;

    if (stk_config.axes_ids.size() > 2)
    {
        yCError(SDLJOYPAD) << "Only for 2-dimensional sticks are supported at the moment";
        return ReturnValue_error_not_implemented_by_device;
    }

    for(size_t i = 0; i < stk_config.axes_ids.size(); i++)
    {
        if(!getAxis(stk_config.axes_ids[i], val)) {return ReturnValue_error_method_failed;}
        value.s1=(val * stk_config.direction[i] * (fabs(val) > stk_config.deadZone));
        value.s2=(val * stk_config.direction[i] * (fabs(val) > stk_config.deadZone));
    }

    if (coordinate_mode == JoypadCtrl_coordinateMode::JypCtrlcoord_POLAR)
    {
        double v0= value.s1;
        double v1= value.s2;
        value.s1 = sqrt(v0 * v0 + v1 * v1);
        value.s2 = atan2(v0, v1);
    }
    return ReturnValue_ok;
}

ReturnValue SDLJoypad:: getTouch(size_t  touch_id, std::vector<yarp::dev::TouchData>& value)
{
    return ReturnValue_error_not_implemented_by_device;
}

ReturnValue SDLJoypad::getHat(size_t hat_id, unsigned char& value)
{
    if(hat_id > m_hatCount - 1)
    {
        yCError(SDLJOYPAD) << "Axis id out of bound!";
        return ReturnValue_error_input_out_of_bounds;
    }
    updateJoypad();
    size_t i;
    for(i = 0; i < m_device.size(); ++i)
    {
        size_t localCount = SDL_JoystickNumHats(m_device[i]);
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
    return ReturnValue_ok;
}

ReturnValue SDLJoypad::getTrackball(size_t  trackball_id, yarp::dev::TrackballData& value)
{
    if(trackball_id > m_ballCount - 1)
    {
        yCError(SDLJOYPAD) << "Trackball id out of bound!";
        return ReturnValue_error_input_out_of_bounds;
    }
    updateJoypad();
    int x, y;
    size_t i;
    for(i = 0; i < m_device.size(); ++i)
    {
        size_t localCount = SDL_JoystickNumBalls(m_device[i]);
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
        return ReturnValue_error_method_failed;
    }

    value.x = x;
    value.y = y;
    return ReturnValue_ok;
}

void SDLJoypad::updateJoypad()
{
    SDL_JoystickUpdate();
}

// --------------------------------------------------------------

bool JoystickActionsHandler::executeAction(int action_id)
{
    if (m_actions.find(action_id) != m_actions.end())
    {
        yCInfo(SDLJOYPAD) << "executing script" << action_id << ":" << m_actions[action_id];
        int ret = system(m_actions[action_id].c_str());
        YARP_UNUSED(ret);
    }
    else
    {
        yCWarning(SDLJOYPAD) << "no scripts associated to button" << action_id;
        return false;
    }
    return true;
}

bool JoystickActionsHandler::parseActions(const yarp::os::Searchable& cfg, size_t maxButtons, int* count)
{
    int dummy = 0;
    size_t i;
    int& actCount = count ? *count : dummy;
    if(!cfg.check(buttActionGroupName))
    {
        yCInfo(SDLJOYPAD) << "no actions found in the configuration file (no" << buttActionGroupName << "group found)";
        actCount = 0;
        return true;
    }
    Bottle& actionsGroup = cfg.findGroup(buttActionGroupName);

    if(!actionsGroup.size())
    {
        yCError(SDLJOYPAD) << "no action found under" << buttActionGroupName << "group";
        actCount = 0;
        return false;
    }

    for(i = 1; i < actionsGroup.size(); i++)
    {
        if(!actionsGroup.get(i).isList())
        {
            yDebug() << "error parsing cfg";
            return false;
        }

        Bottle& keyvalue         = *actionsGroup.get(i).asList();
        yDebug() << keyvalue.toString();
        if(maxButtons==0)
        {
            yCError(SDLJOYPAD) << "invalid button count";
            actCount = 0;
            return false;
        }
        if(!keyvalue.get(0).isInt32()                 ||
            keyvalue.get(0).asInt32() < 0             ||
           (unsigned int) keyvalue.get(0).asInt32() > maxButtons-1 ||
           !keyvalue.get(1).isString())
        {
            yCError(SDLJOYPAD) << "Button's actions parameters must be in the format 'unsigned int string' and the button id must be in range";
            actCount = 0;
            return false;
        }
        yCInfo(SDLJOYPAD) << "assigning actions" << keyvalue.get(1).asString() << "to button" << keyvalue.get(0).asInt32();
        m_actions[keyvalue.get(0).asInt32()] = keyvalue.get(1).asString();
    }

    actCount = i;
    yCInfo(SDLJOYPAD) << actCount << "action parsed successfully";
    return true;
}

bool JoystickActionsHandler::actionDetected(int button_id)
{
    if (m_actions.find(button_id) != m_actions.end())
    {
        return true;
    }
    return false;
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "fakeJoypad.h"
#include <cstdio>
#include <yarp/os/LogStream.h>
#include <cmath>
#include <yarp/os/Time.h>
#include <iostream>


using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(FAKEJOYPAD, "yarp.device.FakeJoypad")
}

FakeJoypad::FakeJoypad() = default;

FakeJoypad::~FakeJoypad() = default;

bool FakeJoypad::open(yarp::os::Searchable& rf)
{
    return true;
}

bool FakeJoypad::close()
{
    return true;
}

bool FakeJoypad::getRawAxisCount(unsigned int& axes_count)
{
    axes_count = m_axisCount;
    return true;
}

bool FakeJoypad::getRawButtonCount(unsigned int& button_count)
{
    button_count = m_buttonCount;
    return true;
}

bool FakeJoypad::getRawTrackballCount(unsigned int& trackball_count)
{
    trackball_count = m_ballCount;
    return true;
}

bool FakeJoypad::getRawHatCount(unsigned int& hat_count)
{
    hat_count = m_hatCount;
    return true;
}

bool FakeJoypad::getRawTouchSurfaceCount(unsigned int& touch_count)
{
    touch_count = m_touchCount;
    return true;
}

bool FakeJoypad::getRawStickCount(unsigned int& stick_count)
{
    stick_count = m_stickCount;
    return true;
}

bool FakeJoypad::getRawStickDoF(unsigned int stick_id, unsigned int& DoF)
{
    if(stick_id > m_stickCount)
    {
        yCError(FAKEJOYPAD) << "stick_id out of bounds when calling 'getRawStickDoF' method";
        return false;
    }
    DoF = 2;
    return true;
}

bool FakeJoypad::getRawButton(unsigned int button_id, float& value)
{
    if (button_id > m_buttonCount)
    {
        yCError(FAKEJOYPAD) << "stick_id out of bounds when calling 'getRawButton' method";
        return false;
    }
    value = button_id*10.0;
    return true;
}

bool FakeJoypad::getRawAxis(unsigned int axis_id, double& value)
{
    if (axis_id > m_axisCount)
    {
        yCError(FAKEJOYPAD) << "axis_id out of bounds when calling 'getRawAxis' method";
        return false;
    }
    value = axis_id * 10.0;
    return true;
}

bool FakeJoypad::getRawStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode)
{
    if (stick_id > m_stickCount)
    {
        yCError(FAKEJOYPAD) << "stick_id out of bounds when calling 'getRawStickDoF' method";
        return false;
    }
    value = stick_id * 10;
    return true;
}

bool FakeJoypad::getRawTouch(unsigned int touch_id, yarp::sig::Vector& value)
{
    if (touch_id > m_touchCount)
    {
        yCError(FAKEJOYPAD) << "touch_id out of bounds when calling 'getRawTouch' method";
        return false;
    }
    value.resize(3);
    value[0] = (double)(touch_id) * 10.0;
    value[1] = (double)(touch_id) * 11.0;
    value[2] = (double)(touch_id) * 12.0;
    return true;
}

bool FakeJoypad::getRawHat(unsigned int hat_id, unsigned char& value)
{
    if (hat_id > m_hatCount)
    {
        yCError(FAKEJOYPAD) << "hat_id out of bounds when calling 'getRawHat' method";
        return false;
    }
    value = hat_id * 10;
    return true;
}

bool FakeJoypad::getRawTrackball(unsigned int trackball_id, yarp::sig::Vector& value)
{
    if (trackball_id > m_ballCount)
    {
        yCError(FAKEJOYPAD) << "trackball_id out of bounds when calling 'getRawTrackball' method";
        return false;
    }

    value.resize(2);
    value[0] = (double)(trackball_id)*10;
    value[1] = (double)(trackball_id)*11;
    return true;
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FakeJoypad.h"
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

FakeJoypad::FakeJoypad()
{
    m_data_touch.resize(2);
}

FakeJoypad::~FakeJoypad()
{
}

bool FakeJoypad::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) {return false;}

    return true;
}

bool FakeJoypad::close()
{
    return true;
}

ReturnValue FakeJoypad::getAxisCount(size_t& axes_count)
{
    axes_count = m_axisCount;
    return ReturnValue_ok;
}

ReturnValue FakeJoypad::getButtonCount(size_t& button_count)
{
    button_count = m_buttonCount;
    return ReturnValue_ok;
}

ReturnValue FakeJoypad::getTrackballCount(size_t& trackball_count)
{
    trackball_count = m_ballCount;
    return ReturnValue_ok;
}

ReturnValue FakeJoypad::getHatCount(size_t& hat_count)
{
    hat_count = m_hatCount;
    return ReturnValue_ok;
}

ReturnValue FakeJoypad::getTouchSurfaceCount(size_t& touch_count)
{
    touch_count = m_touchCount;
    return ReturnValue_ok;
}

ReturnValue FakeJoypad::getStickCount(size_t& stick_count)
{
    stick_count = m_stickCount;
    return ReturnValue_ok;
}

ReturnValue FakeJoypad::getStickDoF(size_t stick_id, size_t& DoF)
{
    if(stick_id > m_stickCount)
    {
        yCError(FAKEJOYPAD) << "stick_id out of bounds when calling 'getStickDoF' method";
        return ReturnValue_error_input_out_of_bounds;
    }
    DoF = 2;
    return ReturnValue_ok;
}

ReturnValue FakeJoypad::getButton(size_t button_id, double& value)
{
    if (button_id > m_buttonCount)
    {
        yCError(FAKEJOYPAD) << "stick_id out of bounds when calling 'getButton' method";
        return ReturnValue_error_input_out_of_bounds;
    }
    value = button_id*11.0;
    return ReturnValue_ok;
}

ReturnValue FakeJoypad::getAxis(size_t axis_id, double& value)
{
    if (axis_id > m_axisCount)
    {
        yCError(FAKEJOYPAD) << "axis_id out of bounds when calling 'getAxis' method";
        return ReturnValue_error_input_out_of_bounds;
    }
    value = axis_id * 10.0;
    return ReturnValue_ok;
}

ReturnValue FakeJoypad::getStick(size_t stick_id, yarp::dev::StickData& value, JoypadCtrl_coordinateMode coordinate_mode)
{
    if (stick_id > m_stickCount)
    {
        yCError(FAKEJOYPAD) << "stick_id out of bounds when calling 'getStickDoF' method";
        return ReturnValue_error_input_out_of_bounds;
    }
    if (coordinate_mode == yarp::dev::IJoypadController::JoypadCtrl_coordinateMode::JypCtrlcoord_CARTESIAN)
    {
        value.s1 = m_data_stick.s1 = stick_id * 100;
        value.s2 = m_data_stick.s2 = stick_id * 101;
    }
    else
    {
        value.s1 = m_data_stick.s1 = stick_id * 102;
        value.s2 = m_data_stick.s2 = stick_id * 103;
    }
    return ReturnValue_ok;
}

ReturnValue FakeJoypad::getTouch(size_t touch_id, std::vector<yarp::dev::TouchData>& value)
{
    if (touch_id > m_touchCount)
    {
        yCError(FAKEJOYPAD) << "touch_id out of bounds when calling 'getTouch' method";
        return ReturnValue_error_input_out_of_bounds;
    }
    value.clear();
    value.resize(2);
    value[0].x = m_data_touch[0].x = (double)(touch_id) * 110.0;
    value[0].y = m_data_touch[0].y = (double)(touch_id) * 111.0;
    value[1].x = m_data_touch[1].x = (double)(touch_id) * 112.0;
    value[1].y = m_data_touch[1].y = (double)(touch_id) * 113.0;
    return ReturnValue_ok;
}

ReturnValue FakeJoypad::getHat(size_t hat_id, unsigned char& value)
{
    if (hat_id > m_hatCount)
    {
        yCError(FAKEJOYPAD) << "hat_id out of bounds when calling 'getHat' method";
        return ReturnValue_error_input_out_of_bounds;
    }
    value = hat_id * 12;
    return ReturnValue_ok;
}

ReturnValue FakeJoypad::getTrackball(size_t trackball_id, yarp::dev::TrackballData& value)
{
    if (trackball_id > m_ballCount)
    {
        yCError(FAKEJOYPAD) << "trackball_id out of bounds when calling 'getTrackball' method";
        return ReturnValue_error_input_out_of_bounds;
    }

    value.x = m_data_trackBall.x = (double)(trackball_id)*120;
    value.y = m_data_trackBall.y = (double)(trackball_id)*121;
    return ReturnValue_ok;
}

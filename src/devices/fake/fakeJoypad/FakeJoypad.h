/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef FAKEJOYPAD_H
#define FAKEJOYPAD_H

#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/DeviceDriver.h>
#include <vector>
#include "FakeJoypad_ParamsParser.h"

class FakeJoypad :
        public yarp::dev::IJoypadEventDriven,
        //public yarp::dev::IJoypadController,
        public yarp::dev::DeviceDriver,
        public FakeJoypad_ParamsParser
{
    unsigned int               m_buttonCount {4};
    unsigned int               m_axisCount {4};
    unsigned int               m_stickCount {4};
    unsigned int               m_touchCount {4};
    unsigned int               m_ballCount {4};
    unsigned int               m_hatCount {4};

public:
    FakeJoypad();
    virtual ~FakeJoypad();

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //IJoypadController
    bool getRawAxisCount(unsigned int& axis_count) override;

    bool getRawButtonCount(unsigned int& button_count) override;

    bool getRawHatCount(unsigned int& hat_count) override;

    bool getRawTrackballCount(unsigned int& trackball_count) override;

    bool getRawTouchSurfaceCount(unsigned int& touch_count) override;

    bool getRawStickCount(unsigned int& stick_count) override;

    bool getRawStickDoF(unsigned int  stick_id, unsigned int& DoF) override;

    bool getRawButton(unsigned int  button_id, float& value) override;

    bool getRawTrackball(unsigned int  trackball_id, yarp::sig::Vector& value) override;

    bool getRawHat(unsigned int  hat_id, unsigned char& value) override;

    bool getRawAxis(unsigned int  axis_id, double& value) override;

    bool getRawStick(unsigned int  stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode) override;

    bool getRawTouch(unsigned int  touch_id, yarp::sig::Vector& value) override;
};


#endif //#define FAKEJOYPAD_H

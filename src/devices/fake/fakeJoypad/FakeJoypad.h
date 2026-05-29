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

 /**
 * @ingroup dev_impl_fake
 * \brief `FakeJoypad` : fake device implementing the device interface
 *
 * Parameters required by this device are shown in class: FakeJoypad_ParamsParser
 *
 */
class FakeJoypad :
        public yarp::dev::IJoypadController,
        public yarp::dev::DeviceDriver,
        public FakeJoypad_ParamsParser
{
    size_t                     m_buttonCount {4};
    size_t                     m_axisCount {4};
    size_t                     m_stickCount {4};
    size_t                     m_touchCount {4};
    size_t                     m_ballCount {4};
    size_t                     m_hatCount {4};

    yarp::dev::TrackballData           m_data_trackBall;
    std::vector<yarp::dev::TouchData>  m_data_touch;
    yarp::dev::StickData               m_data_stick;

public:
    FakeJoypad();
    virtual ~FakeJoypad();

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //IJoypadController
    yarp::dev::ReturnValue getAxisCount(size_t& axis_count) override;
    yarp::dev::ReturnValue getButtonCount(size_t& button_count) override;
    yarp::dev::ReturnValue getHatCount(size_t& hat_count) override;
    yarp::dev::ReturnValue getTrackballCount(size_t& trackball_count) override;
    yarp::dev::ReturnValue getTouchSurfaceCount(size_t& touch_count) override;
    yarp::dev::ReturnValue getStickCount(size_t& stick_count) override;
    yarp::dev::ReturnValue getStickDoF(size_t  stick_id, size_t& DoF) override;
    yarp::dev::ReturnValue getButton(size_t  button_id, double& value) override;
    yarp::dev::ReturnValue getTrackball(size_t  trackball_id, yarp::dev::TrackballData& value) override;
    yarp::dev::ReturnValue getHat(size_t  hat_id, unsigned char& value) override;
    yarp::dev::ReturnValue getAxis(size_t  axis_id, double& value) override;
    yarp::dev::ReturnValue getStick(size_t  stick_id, yarp::dev::StickData& value, JoypadCtrl_coordinateMode coordinate_mode) override;
    yarp::dev::ReturnValue getTouch(size_t  touch_id, std::vector<yarp::dev::TouchData>& value) override;
};


#endif //#define FAKEJOYPAD_H

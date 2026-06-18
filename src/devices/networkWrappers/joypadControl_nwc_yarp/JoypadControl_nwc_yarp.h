/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_JOYPADCONTROL_NWC_YARP_H
#define YARP_DEV_JOYPADCONTROL_NWC_YARP_H

#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PeriodicThread.h>
#include <vector>
#include <mutex>
#include "JoypadControl_nwc_yarp_ParamsParser.h"

#include "yarp/dev/IJoypadControlMsgs.h"
#include <yarp/dev/AxisDataList.h>
#include <yarp/dev/HatsDataList.h>
#include <yarp/dev/TouchesDataList.h>
#include <yarp/dev/StickDataList.h>
#include <yarp/dev/ButtonDataList.h>
#include <yarp/dev/TrackballDataList.h>
#include <yarp/dev/AllJoyData.h>

#define DEFAULT_THREAD_PERIOD 10

template <class T>
class ReaderPort :
        public yarp::os::BufferedPort<T>
{
   private:
   T data;
   std::mutex mtx;
   bool test=true;

   public:
    using yarp::os::BufferedPort<T>::onRead;
    void onRead(T &v) override
    {
        if constexpr (std::is_same_v<T, yarp::dev::AxisDataList>)
        {
            test=!test;
        }
        std::lock_guard<std::mutex> lock(mtx);
        data = v;
    }

    T getData()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return data;
    }
};

/**
* @ingroup dev_impl_nwc_yarp
*
* \brief `JoypadControl_nwc_yarp`: joypad input network wrapper on client side
*
* \section JoypadControl_nwc_yarp Description of input parameters
*
* Parameters required by this device are shown in class: JoypadControl_nwc_yarp_ParamsParser
*/
class JoypadControl_nwc_yarp :
        public yarp::dev::IJoypadController,
        public yarp::dev::DeviceDriver,
        public JoypadControl_nwc_yarp_ParamsParser
{
private:
    //---------------properties
    yarp::os::Port                                  m_rpcPort;
    ReaderPort<yarp::dev::ButtonDataList>           m_buttonsPort;
    ReaderPort<yarp::dev::AxisDataList>             m_axisPort;
    ReaderPort<yarp::dev::TrackballDataList>        m_trackballPort;
    ReaderPort<yarp::dev::TouchesDataList>          m_touchPort;
    ReaderPort<yarp::dev::HatsDataList>             m_hatsPort;
    ReaderPort<yarp::dev::StickDataList>            m_stickPort;
    ReaderPort<yarp::dev::AllJoyData>               m_allJoyDataPort;
    std::vector<size_t>                             m_stickDof;
    yarp::dev::IJoypadControlMsgs                   m_rpcMsgs;

    //--------------method
    bool getJoypadInfo();

public:
    JoypadControl_nwc_yarp();
    JoypadControl_nwc_yarp(const JoypadControl_nwc_yarp&) = delete;
    JoypadControl_nwc_yarp(JoypadControl_nwc_yarp&&) = delete;
    JoypadControl_nwc_yarp& operator=(const JoypadControl_nwc_yarp&) = delete;
    JoypadControl_nwc_yarp& operator=(JoypadControl_nwc_yarp&&) = delete;
    ~JoypadControl_nwc_yarp() override = default;

    //number of available devices
    size_t m_axis_count=0;
    size_t m_button_count=0;
    size_t m_trackball_count=0;
    size_t m_hat_count=0;
    size_t m_touch_count=0;
    size_t m_stick_count=0;

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //IJoypadController;
    yarp::dev::ReturnValue getAxisCount(size_t& axis_count) override;
    yarp::dev::ReturnValue getButtonCount(size_t& button_count) override;
    yarp::dev::ReturnValue getTrackballCount(size_t& Trackball_count) override;
    yarp::dev::ReturnValue getHatCount(size_t& Hat_count) override;
    yarp::dev::ReturnValue getTouchSurfaceCount(size_t& touch_count) override;
    yarp::dev::ReturnValue getStickCount(size_t& stick_count) override;
    yarp::dev::ReturnValue getStickDoF(size_t stick_id, size_t& DoF) override;
    yarp::dev::ReturnValue getButton(size_t button_id, double& value) override;
    yarp::dev::ReturnValue getTrackball(size_t trackball_id, yarp::dev::TrackballData& value) override;
    yarp::dev::ReturnValue getHat(size_t hat_id, unsigned char& value) override;
    yarp::dev::ReturnValue getAxis(size_t axis_id, double& value) override;
    yarp::dev::ReturnValue getStick(size_t stick_id, yarp::dev::StickData& value, JoypadCtrl_coordinateMode coordinate_mode) override;
    yarp::dev::ReturnValue getTouch(size_t touch_id, std::vector<yarp::dev::TouchData>& value) override;
};

#endif

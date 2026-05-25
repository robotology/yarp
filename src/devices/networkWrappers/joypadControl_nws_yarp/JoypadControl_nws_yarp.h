/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_JOYPADCONTROLSERVER_JOYPADCONTROLSERVER_H
#define YARP_DEV_JOYPADCONTROLSERVER_JOYPADCONTROLSERVER_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/RPCServer.h>

#include <map>

#include <JoypadControlNetUtils.h>
#include "JoypadControl_nws_yarp_ParamsParser.h"
#include <yarp/dev/IJoypadControlMsgs.h>
#include <yarp/dev/AxisDataList.h>
#include <yarp/dev/HatsDataList.h>
#include <yarp/dev/TouchesDataList.h>
#include <yarp/dev/StickDataList.h>
#include <yarp/dev/ButtonDataList.h>
#include <yarp/dev/TrackballDataList.h>

class IJoypadControlRPCd;

/**
 * @ingroup dev_impl_wrapper
 *
 * `JoypadControlServer`: joypad input network wrapper on server side
 *
 * \section JoypadControlServer Description of input parameters
 *
 * Parameters required by this device are shown in class: JoypadControlServer_ParamsParser
 */
class JoypadControl_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PeriodicThread,
        public yarp::os::PortReader,
        public JoypadControl_nws_yarp_ParamsParser
{
    typedef yarp::dev::IJoypadController::JoypadCtrl_coordinateMode coordsMode;

    std::unique_ptr<IJoypadControlRPCd>   m_RPCMsgs;

    yarp::dev::IJoypadController*        m_IJoypad = nullptr;

    yarp::os::RpcServer                  m_rpcPort;

    yarp::os::BufferedPort<yarp::dev::AxisDataList>        m_portAxes;
    yarp::os::BufferedPort<yarp::dev::StickDataList>       m_portSticks;
    yarp::os::BufferedPort<yarp::dev::TouchesDataList>     m_portTouches;
    yarp::os::BufferedPort<yarp::dev::ButtonDataList>      m_portButtons;
    yarp::os::BufferedPort<yarp::dev::HatsDataList>        m_portHats;
    yarp::os::BufferedPort<yarp::dev::TrackballDataList>   m_portTrackballs;

    size_t                               m_numberOfAxes=0;
    size_t                               m_numberOfSticks=0;
    size_t                               m_numberOfTouchSurfaces=0;
    size_t                               m_numberOfButtons=0;
    size_t                               m_numberOfHats=0;
    size_t                               m_numberOfTrackballs=0;

    std::vector<double>                       m_curr_axes;
    std::vector<yarp::dev::StickData>         m_curr_sticks;
    yarp::dev::TouchesDataList                m_curr_touches;
    std::vector<double>                       m_curr_buttons;
    std::vector<unsigned char>                m_curr_hats;
    std::vector<yarp::dev::TrackballData>     m_curr_trackballs;

    std::vector<double>                       m_last_axes;
    std::vector<yarp::dev::StickData>         m_last_sticks;
    yarp::dev::TouchesDataList                m_last_touches;
    std::vector<double>                       m_last_buttons;
    std::vector<unsigned char>                m_last_hats;
    std::vector<yarp::dev::TrackballData>     m_last_trackballs;

    yarp::os::BufferedPort<JoyData>      m_allPort; //TODO: single port purpose

    //polar mode is used in streaming.
    const coordsMode                     m_coordsMode = yarp::dev::IJoypadController::JoypadCtrl_coordinateMode::JypCtrlcoord_POLAR;

    bool openPorts();

public:
    JoypadControl_nws_yarp();
    JoypadControl_nws_yarp(const JoypadControl_nws_yarp&) = delete;
    JoypadControl_nws_yarp(JoypadControl_nws_yarp&&) = delete;
    JoypadControl_nws_yarp& operator=(const JoypadControl_nws_yarp&) = delete;
    JoypadControl_nws_yarp& operator=(JoypadControl_nws_yarp&&) = delete;
    ~JoypadControl_nws_yarp() override;

    bool open(yarp::os::Searchable& params) override;
    bool close() override;
    bool attach(yarp::dev::PolyDriver* poly) override;
    bool detach() override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

    //rpc port
    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif

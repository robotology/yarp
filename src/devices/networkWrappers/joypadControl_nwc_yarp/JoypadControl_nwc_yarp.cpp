/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "JoypadControl_nwc_yarp.h"
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <tuple>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(JOYPADCONTROLCLIENT, "yarp.device.JoypadControl_nwc_yarp")
}

JoypadControl_nwc_yarp::JoypadControl_nwc_yarp()
{
}

bool JoypadControl_nwc_yarp::getJoypadInfo()
{
    bool v= true;
    v&=getAxisCount         (m_axis_count);
    v&=getButtonCount       (m_button_count);
    v&=getTrackballCount    (m_trackball_count);
    v&=getHatCount          (m_hat_count);
    v&=getTouchSurfaceCount (m_touch_count);
    v&=getStickCount        (m_stick_count);

    return v;
}

bool JoypadControl_nwc_yarp::open(yarp::os::Searchable& config)
{
    if (!parseParams(config)) { return false; }

    //open the rpc port
    if(!m_rpcPort.open(m_local + "/rpc:o"))
    {
        yCError(JOYPADCONTROLCLIENT) << "Unable to open rpc port.";
        return false;
    }
    yCInfo(JOYPADCONTROLCLIENT) << "rpc port opened. starting the handshake";

    //connection for the rpc port
    if(!yarp::os::NetworkBase::connect(m_local + "/rpc:o", m_remote + "/rpc:i"))
    {
        yCError(JOYPADCONTROLCLIENT) << "Handshake failed. unable to connect to remote port" << m_remote + "/rpc:i";
        return false;
    }

    if (!m_rpcMsgs.yarp().attachAsClient(m_rpcPort))
    {
       yCError(JOYPADCONTROLCLIENT, "Error! Cannot attach the port as a client");
       return false;
    }

    // Check the protocol version
    if (!m_rpcMsgs.checkProtocolVersion()) {
        return false;
    }

    //Get basic infos from the joypad
    if(!getJoypadInfo())
    {
        yCError(JOYPADCONTROLCLIENT) << "Unable to get joypad info.";
        return false;
    }

    if (m_use_streaming==false) {return true;}

    //The code below is valid only for streaming mode
    if (m_use_all_port)
    {
        bool b = true;
        b &= m_allJoyDataPort.open(m_local + "/allJoyData:i");
        if (!b) {
            yCError(JOYPADCONTROLCLIENT) << "Unable to open local streaming ports";
            close();
            return false;
        }

        b&= yarp::os::NetworkBase::connect(m_remote+"/allJoyData:o", m_local+"/allJoyData:i");
        if (!b) {
            yCError(JOYPADCONTROLCLIENT) << "Unable to connect to remote streaming port";
            close();
            return  false;
        }

        m_allJoyDataPort.useCallback();
    }
    else
    {
        bool b = true;
        b&= m_buttonsPort.open(m_local+"/buttons:i");
        b&= m_axisPort.open(m_local+"/axis:i");
        b&= m_trackballPort.open(m_local+"/trackball:i");
        b&= m_touchPort.open(m_local+"/touch:i");
        b&= m_hatsPort.open(m_local+"/hat:i");
        b&= m_stickPort.open(m_local+"/stick:i");
        if (!b) {
            yCError(JOYPADCONTROLCLIENT) << "Unable to open local streaming ports";
            close();
            return false;
        }

        b&= yarp::os::NetworkBase::connect(m_remote+"/buttons:o",m_local+"/buttons:i");
        b&= yarp::os::NetworkBase::connect(m_remote+"/axis:o",m_local+"/axis:i");
        b&= yarp::os::NetworkBase::connect(m_remote+"/trackball:o",m_local+"/trackball:i");
        b&= yarp::os::NetworkBase::connect(m_remote+"/touch:o",m_local+"/touch:i");
        b&= yarp::os::NetworkBase::connect(m_remote+"/hat:o",m_local+"/hat:i");
        b&= yarp::os::NetworkBase::connect(m_remote+"/stick:o",m_local+"/stick:i");
        if (!b) {
            yCError(JOYPADCONTROLCLIENT) << "Unable to connect to remote streaming port";
            close();
            return  false;
        }

        m_buttonsPort.useCallback();
        m_axisPort.useCallback();
        m_trackballPort.useCallback();
        m_touchPort.useCallback();
        m_hatsPort.useCallback();
        m_stickPort.useCallback();
    }

    return true;
}


ReturnValue JoypadControl_nwc_yarp::getButtonCount(size_t& button_count)
{
    auto ret = m_rpcMsgs.getButtonCount();
    button_count = ret.button_count;
    return ret.ret;
}

ReturnValue JoypadControl_nwc_yarp::getAxisCount(size_t& axis_count)
{
    auto ret = m_rpcMsgs.getAxisCount();
    axis_count = ret.axis_count;
    return ret.ret;
}

ReturnValue JoypadControl_nwc_yarp::getTrackballCount(size_t& Trackball_count)
{
    auto ret = m_rpcMsgs.getTrackballCount();
    Trackball_count = ret.Trackball_count;
    return ret.ret;
}

ReturnValue JoypadControl_nwc_yarp::getHatCount(size_t& Hat_count)
{
    auto ret = m_rpcMsgs.getHatCount();
    Hat_count = ret.Hat_count;
    return ret.ret;
}

ReturnValue JoypadControl_nwc_yarp::getTouchSurfaceCount(size_t& touch_count)
{
    auto ret = m_rpcMsgs.getTouchSurfaceCount();
    touch_count = ret.touch_count;
    return ret.ret;
}

ReturnValue JoypadControl_nwc_yarp::getStickCount(size_t& stick_count)
{
    auto ret = m_rpcMsgs.getStickCount();
    stick_count = ret.stick_count;
    return ret.ret;
}

ReturnValue JoypadControl_nwc_yarp::getStickDoF(size_t stick_id, size_t& DoF)
{
    auto ret = m_rpcMsgs.getStickDoF(stick_id);
    DoF = ret.DoF;
    return ret.ret;
}

ReturnValue JoypadControl_nwc_yarp::getButton(size_t button_id, double& value)
{
    if(!m_use_streaming)
    {
        auto ret = m_rpcMsgs.getButton(button_id);
        value = ret.value;
        return ret.ret;
    }
    else if (m_use_all_port)
    {
        auto tmp = m_allJoyDataPort.getData();
        if(button_id < tmp.ButtonDataVal.size())
        {
            value = tmp.ButtonDataVal[button_id];
            return ReturnValue_ok;
        }
        else
        {
            yCError(JOYPADCONTROLCLIENT) << "GetButton() error. button_id out of bound";
            return ReturnValue_error_method_failed;
        }
    }
    else
    {
        auto tmp = m_buttonsPort.getData();
        if(button_id < tmp.value.size())
        {
            value = tmp.value[button_id];
            return ReturnValue_ok;
        }
        else
        {
            yCError(JOYPADCONTROLCLIENT) << "GetButton() error. button_id out of bound";
            return ReturnValue_error_method_failed;
        }
    }
}

ReturnValue JoypadControl_nwc_yarp::getTrackball(size_t trackball_id, yarp::dev::TrackballData& value)
{
    if(!m_use_streaming)
    {
        auto ret = m_rpcMsgs.getTrackball(trackball_id);
        value = ret.value;
        return ret.ret;
    }
    else if (m_use_all_port)
    {
        auto tmp = m_allJoyDataPort.getData();
        if(trackball_id < tmp.TrackballDataVal.size())
        {
            value = tmp.TrackballDataVal[trackball_id];
            return ReturnValue_ok;
        }
        else
        {
            yCError(JOYPADCONTROLCLIENT) << "GetTrackball() error. trackball_id out of bound";
            return ReturnValue_error_method_failed;
        }
    }
    else
    {
        auto tmp = m_trackballPort.getData();
        if(trackball_id < tmp.value.size())
        {
            value = tmp.value[trackball_id];
            return ReturnValue_ok;
        }
        else
        {
            return ReturnValue_error_method_failed;
        }
    }
}

ReturnValue JoypadControl_nwc_yarp::getHat(size_t hat_id, unsigned char& value)
{
    if(!m_use_streaming)
    {
        auto ret = m_rpcMsgs.getHat(hat_id);
        value = ret.value;
        return ret.ret;
    }
    else if (m_use_all_port)
    {
        auto tmp = m_allJoyDataPort.getData();
        if(hat_id < tmp.HatsDataVal.size())
        {
            value = tmp.HatsDataVal[hat_id];
            return ReturnValue_ok;
        }
        else
        {
            yCError(JOYPADCONTROLCLIENT) << "GetHat() error. hat_id out of bound";
            return ReturnValue_error_method_failed;
        }
    }
    else
    {
        auto tmp = m_hatsPort.getData();
        if (hat_id < tmp.value.size())
        {
            value = tmp.value[hat_id];
            return ReturnValue_ok;
        }
        else
        {
            return ReturnValue_error_method_failed;
        }
    }
}

ReturnValue JoypadControl_nwc_yarp::getAxis(size_t axis_id, double& value)
{
    if(!m_use_streaming)
    {
        auto ret = m_rpcMsgs.getAxis(axis_id);
        value = ret.value;
        return ret.ret;
    }
    else
    {
        auto tmp = m_axisPort.getData();
        if(axis_id < tmp.value.size())
        {
            value = tmp.value[axis_id];
            return ReturnValue_ok;
        }
        else if (m_use_all_port)
        {
            auto tmp = m_allJoyDataPort.getData();
            if(axis_id < tmp.AxisDataVal.size())
            {
                value = tmp.AxisDataVal[axis_id];
                return ReturnValue_ok;
            }
            else
            {
                yCError(JOYPADCONTROLCLIENT) << "GetAxis() error. axis_id out of bound";
                return ReturnValue_error_method_failed;
            }
        }
        else
        {
            yCError(JOYPADCONTROLCLIENT) << "GetAxis() error. Axis_id out of bound";
            return ReturnValue_error_method_failed;
        }
    }
}

ReturnValue JoypadControl_nwc_yarp::getStick(size_t stick_id, yarp::dev::StickData& value, JoypadCtrl_coordinateMode mode)
{
    if(!m_use_streaming)
    {
        auto ret = m_rpcMsgs.getStick(stick_id,mode);
        value = ret.value;
        return ret.ret;
    }
    else if (m_use_all_port)
    {
        //Only the polar mode is available in streaming.
        //If cartesian mode is requested, switch back to rpc.
        if (mode == yarp::dev::IJoypadController::JoypadCtrl_coordinateMode::JypCtrlcoord_CARTESIAN)
        {
            auto ret = m_rpcMsgs.getStick(stick_id,mode);
            value = ret.value;
            return ret.ret;
        }
        auto tmp = m_allJoyDataPort.getData();
        if(stick_id < tmp.StickDataVal.size())
        {
            value = tmp.StickDataVal[stick_id];
            return ReturnValue_ok;
        }
        else
        {
            yCError(JOYPADCONTROLCLIENT) << "getStick() error. stick_id out of bound";
            return ReturnValue_error_method_failed;
        }
    }
    else
    {
        //Only the polar mode is available in streaming.
        //If cartesian mode is requested, switch back to rpc.
        if (mode == yarp::dev::IJoypadController::JoypadCtrl_coordinateMode::JypCtrlcoord_CARTESIAN)
        {
            auto ret = m_rpcMsgs.getStick(stick_id,mode);
            value = ret.value;
            return ret.ret;
        }
        auto tmp = m_stickPort.getData();
        if(stick_id < tmp.value.size())
        {
            value = tmp.value[stick_id];
            return ReturnValue_ok;
        }
        else
        {
            yCError(JOYPADCONTROLCLIENT) << "GetAxis() error. stick_id out of bound";
            return ReturnValue_error_method_failed;
        }

        return ReturnValue_ok;
    }
}

ReturnValue JoypadControl_nwc_yarp::getTouch(size_t touch_id, std::vector<yarp::dev::TouchData>& value)
{
    value.clear();
    if(!m_use_streaming)
    {
        auto ret = m_rpcMsgs.getTouch(touch_id);
        value = ret.value;
        return ret.ret;
    }
    else if (m_use_all_port)
    {
        auto tmp = m_allJoyDataPort.getData();
        if(touch_id < tmp.TouchDataVal.size())
        {
            value = tmp.TouchDataVal[touch_id].touches;
            return ReturnValue_ok;
        }
        else
        {
            yCError(JOYPADCONTROLCLIENT) << "GetTouch() error. touch_id out of bound";
            return ReturnValue_error_method_failed;
        }
    }
    else
    {
        auto temp = m_touchPort.getData();
        if(touch_id < temp.value.size())
        {
            value.resize(temp.value[touch_id].touches.size());
            for (size_t i = 0; i < value.size(); i++)
            {
                value[i] = temp.value[touch_id].touches[i];
            }
            return ReturnValue_ok;
        }
        else
        {
            return ReturnValue_error_method_failed;
        }
    }
}

bool JoypadControl_nwc_yarp::close()
{
    m_buttonsPort.close();
    m_axisPort.close();
    m_trackballPort.close();
    m_touchPort.close();
    m_hatsPort.close();
    m_stickPort.close();

    m_rpcPort.close();

    m_allJoyDataPort.close();
    return true;
}

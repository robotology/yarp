/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "JoypadControl_nws_yarp.h"
#include <algorithm>
#include <sstream>
#include <limits>

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#define DEFAULT_THREAD_PERIOD   0.010 //s

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(JOYPADCONTROLSERVER, "yarp.device.JoypadControl_nws_yarp")
}

template <class T>
inline void cat(yarp::sig::VectorOf<T>& a, const yarp::sig::VectorOf<T>& b)
{
    for (size_t i = 0; i < b.size(); i++)
    {
        a.push_back(b[i]);
    }
}

class IJoypadControlRPCd : public yarp::dev::IJoypadControlMsgs
{
    private:
    mutable std::mutex              m_mutex;
    yarp::dev::IJoypadController*   m_iJoy = nullptr;

    public:
    IJoypadControlRPCd(yarp::dev::IJoypadController* interfaces)
    {
        m_iJoy = interfaces;
    }

    //Methods
    return_getAxisCount getAxisCount() override;
    return_getButtonCount getButtonCount() override;
    return_getTrackballCount getTrackballCount() override;
    return_getHatCount getHatCount() override;
    return_getTouchSurfaceCount getTouchSurfaceCount() override;
    return_getStickCount getStickCount() override;
    return_getStickDoF getStickDoF(const std::int32_t stick_id) override;
    return_getButton getButton(const std::int32_t button_id) override;
    return_getTrackball getTrackball(const std::int32_t trackball_id) override;
    return_getHat getHat(const std::int32_t hat_id) override;
    return_getAxis getAxis(const std::int32_t axis_id) override;
    return_getStick getStick(const std::int32_t stick_id, yarp::dev::IJoypadController::JoypadCtrl_coordinateMode mode) override;
    return_getTouch getTouch(const std::int32_t touch_id) override;
};

return_getAxisCount IJoypadControlRPCd::getAxisCount()
{
    return_getAxisCount ret;

    if (!m_iJoy) {
        yCError(JOYPADCONTROLSERVER, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    size_t temp=0;
    ret.ret = m_iJoy->getAxisCount(temp);
    ret.axis_count = temp;
    return ret;
}

return_getButtonCount IJoypadControlRPCd::getButtonCount()
{
    return_getButtonCount ret;

    if (!m_iJoy) {
        yCError(JOYPADCONTROLSERVER, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    size_t temp=0;
    ret.ret = m_iJoy->getButtonCount(temp);
    ret.button_count = temp;
    return ret;
}

return_getTrackballCount IJoypadControlRPCd::getTrackballCount()
{
    return_getTrackballCount ret;

    if (!m_iJoy) {
        yCError(JOYPADCONTROLSERVER, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    size_t temp=0;
    ret.ret = m_iJoy->getTrackballCount(temp);
    ret.Trackball_count= temp;
    return ret;
}

return_getHatCount IJoypadControlRPCd::getHatCount()
{
    return_getHatCount ret;

    if (!m_iJoy) {
        yCError(JOYPADCONTROLSERVER, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    size_t temp=0;
    ret.ret = m_iJoy->getHatCount(temp);
    ret.Hat_count= temp;
    return ret;
}

return_getTouchSurfaceCount IJoypadControlRPCd::getTouchSurfaceCount()
{
    return_getTouchSurfaceCount ret;

    if (!m_iJoy) {
        yCError(JOYPADCONTROLSERVER, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    size_t temp=0;
    ret.ret = m_iJoy->getTouchSurfaceCount(temp);
    ret.touch_count= temp;
    return ret;
}

return_getStickCount IJoypadControlRPCd::getStickCount()
{
    return_getStickCount ret;

    if (!m_iJoy) {
        yCError(JOYPADCONTROLSERVER, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    size_t temp=0;
    ret.ret = m_iJoy->getStickCount(temp);
    ret.stick_count=temp;
    return ret;
}

return_getButton IJoypadControlRPCd::getButton(const std::int32_t id)
{
    return_getButton ret;

    if (!m_iJoy) {
        yCError(JOYPADCONTROLSERVER, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iJoy->getButton(id, ret.value);
    return ret;
}

return_getTrackball IJoypadControlRPCd::getTrackball(const std::int32_t id)
{
    return_getTrackball ret;

    if (!m_iJoy) {
        yCError(JOYPADCONTROLSERVER, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    size_t temp=0;
    ret.ret = m_iJoy->getTrackball(id, ret.value);
    return ret;
}

return_getStickDoF IJoypadControlRPCd::getStickDoF(const std::int32_t id)
{
    return_getStickDoF ret;

    if (!m_iJoy) {
        yCError(JOYPADCONTROLSERVER, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    size_t temp=0;
    ret.ret = m_iJoy->getStickDoF(id, temp);
    ret.DoF= temp;
    return ret;
}

return_getHat IJoypadControlRPCd::getHat(const std::int32_t id)
{
    return_getHat ret;

    if (!m_iJoy) {
        yCError(JOYPADCONTROLSERVER, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    unsigned char temp = 0;
    ret.ret = m_iJoy->getHat(id, temp);
    ret.value= temp;
    return ret;
}

return_getAxis IJoypadControlRPCd::getAxis(const std::int32_t id)
{
    return_getAxis ret;

    if (!m_iJoy) {
        yCError(JOYPADCONTROLSERVER, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }


    ret.ret = m_iJoy->getAxis(id, ret.value);
    return ret;
}

return_getStick IJoypadControlRPCd::getStick(const std::int32_t id,yarp::dev::IJoypadController::JoypadCtrl_coordinateMode mode)
{
    return_getStick ret;

    if (!m_iJoy) {
        yCError(JOYPADCONTROLSERVER, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iJoy->getStick(id, ret.value, mode);
    return ret;
}

return_getTouch IJoypadControlRPCd::getTouch(const std::int32_t id)
{
    return_getTouch ret;

    if (!m_iJoy) {
        yCError(JOYPADCONTROLSERVER, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iJoy->getTouch(id, ret.value);
    return ret;
}

//-------------------------------------------------------------

JoypadControl_nws_yarp::JoypadControl_nws_yarp() : PeriodicThread(DEFAULT_THREAD_PERIOD)
{
}

JoypadControl_nws_yarp::~JoypadControl_nws_yarp()
{
}

bool JoypadControl_nws_yarp::open(yarp::os::Searchable& config)
{
    if (!parseParams(config)) { return false; }

    if(m_use_separate_ports == false)
    {
        yCError(JOYPADCONTROLSERVER) << "Single port mode not supported at the moment";
        return false;
    }

    yCInfo(JOYPADCONTROLSERVER) << "Running, waiting for attach...";
    return true;
}

bool JoypadControl_nws_yarp::attach(PolyDriver* poly)
{
    if (poly) {
        poly->view(m_IJoypad);
    }

    if(m_IJoypad == nullptr)
    {
        yCError(JOYPADCONTROLSERVER) << "Attached device has no valid IJoypadController interface.";
        return false;
    }

    if (!openPorts())
    {
        yCError(JOYPADCONTROLSERVER) << "Unable to open ports";
        return false;
    }

    m_RPCMsgs = std::make_unique<IJoypadControlRPCd>(this->m_IJoypad);

    PeriodicThread::setPeriod(m_period);
    if (!PeriodicThread::start()) {
        return false;
    }

    return true;
}

bool JoypadControl_nws_yarp::detach()
{
    if (yarp::os::PeriodicThread::isRunning()) {
        yarp::os::PeriodicThread::stop();
    }

    m_IJoypad = nullptr;
    return true;
}

bool JoypadControl_nws_yarp::threadInit()
{
    bool v= true;
    v&=m_IJoypad->getAxisCount         (m_numberOfAxes);
    v&=m_IJoypad->getButtonCount       (m_numberOfButtons);
    v&=m_IJoypad->getTrackballCount    (m_numberOfTrackballs);
    v&=m_IJoypad->getHatCount          (m_numberOfHats);
    v&=m_IJoypad->getTouchSurfaceCount (m_numberOfTouchSurfaces);
    v&=m_IJoypad->getStickCount        (m_numberOfSticks);
    if (!v) return false;

    m_curr_axes.resize             (m_numberOfAxes);
    m_curr_buttons.resize          (m_numberOfButtons);
    m_curr_trackballs.resize       (m_numberOfTrackballs);
    m_curr_hats.resize             (m_numberOfHats);
    m_curr_sticks.resize           (m_numberOfSticks);
    m_curr_touches.value.resize    (m_numberOfTouchSurfaces);

    m_last_axes.resize             (m_numberOfAxes);
    m_last_buttons.resize          (m_numberOfButtons);
    m_last_trackballs.resize       (m_numberOfTrackballs);
    m_last_hats.resize             (m_numberOfHats);
    m_last_sticks.resize           (m_numberOfSticks);
    m_last_touches.value.resize    (m_numberOfTouchSurfaces);

    return true;
}

void JoypadControl_nws_yarp::threadRelease()
{
}

bool JoypadControl_nws_yarp::openPorts()
{
    if(!m_IJoypad)
    {
        return false;
    }

    if (!m_rpcPort.open(m_name + "/rpc:i"))
    {
        yCError(JOYPADCONTROLSERVER) << "Unable to open rpc Port" << (m_name+"/rpc:o");
        return false;
    }
    m_rpcPort.setReader(*this);

    if(m_use_separate_ports)
    {
        bool b=true;
        b &= m_portAxes.open(m_name+"/axis:o");
        b &= m_portButtons.open(m_name+"/buttons:o");
        b &= m_portSticks.open(m_name+"/stick:o");
        b &= m_portTouches.open(m_name+"/touch:o");
        b &= m_portTrackballs.open(m_name+"/trackball:o");
        b &= m_portHats.open(m_name+"/hat:o");
        if (!b)
        {
            yCError(JOYPADCONTROLSERVER) << "Unable to streaming ports";
            return false;
        }
    }
    else
    {
        //m_allPort.open(m_name + "/joydata:o");
        yCError(JOYPADCONTROLSERVER) << "All ports not implemented";
        return false;
    }

    return true;
}

template <class T>
bool VectorsDifferent(const std::vector<T>& a, const std::vector<T>& b)
{
    return a.size() != b.size() ||
           !std::equal(a.begin(), a.end(), b.begin());
}


template <class T, class Compare>
bool VectorsDifferentC(const std::vector<T>& a, const std::vector<T>& b, Compare comp)
{
    return a.size() != b.size() ||
           !std::equal(a.begin(), a.end(), b.begin(), comp);
}

inline std::string printVector(const std::vector<double>& v)
{
    std::ostringstream oss;
    oss.precision(std::numeric_limits<double>::max_digits10);
    oss << '(';
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        oss << ' ' << v[i];
    }
    oss << ')';
    return oss.str();
}

void JoypadControl_nws_yarp::run()
{
    std::string       profile_message;

    if(m_use_separate_ports)
    {
        //1 Buttons
        {
            bool write;
            write = m_portButtons.getOutputCount()>0;
            auto& b = m_portButtons.prepare();
            b.value.clear();
            for(size_t i = 0; i < m_numberOfButtons; ++i)
            {
                if (!m_IJoypad->getButton(i, m_curr_buttons[i]))
                {
                    write = false;
                    break;
                }
            }

            if (write)
            {
                if (m_publish_on_event)
                {
                   if (VectorsDifferent(m_curr_buttons,m_last_buttons))
                   {
                      b.value = m_curr_buttons;
                      m_portButtons.write();
                      m_last_buttons = m_curr_buttons;
                   }
                }
                else
                {
                   b.value = m_curr_buttons;
                   m_portButtons.write();
                   m_last_buttons = m_curr_buttons;
                }
            }
        }

        //2 Hats
        {
            bool write;

            write = m_portHats.getOutputCount()>0;
            auto& b = m_portHats.prepare();
            b.value.clear();
            for(size_t i = 0; i < m_numberOfHats; ++i)
            {
                if (!m_IJoypad->getHat(i, m_curr_hats[i]))
                {
                    write = false;
                    break;
                }
            }

            if (write)
            {
                if (m_publish_on_event)
                {
                   if (VectorsDifferent(m_curr_hats,m_last_hats))
                   {
                      b.value.resize(m_curr_hats.size());
                      std::transform(m_curr_hats.begin(), m_curr_hats.end(), b.value.begin(), [](unsigned char c) { return static_cast<char>(c); });
                      //b.value = m_curr_hats; //using std::transform instead
                      m_portHats.write();
                      m_last_hats = m_curr_hats;
                   }
                }
                else
                {
                   b.value.resize(m_curr_hats.size());
                   std::transform(m_curr_hats.begin(), m_curr_hats.end(), b.value.begin(), [](unsigned char c) { return static_cast<char>(c); });
                   //b.value = m_curr_hats; //using std::transform instead
                   m_portHats.write();
                   m_last_hats = m_curr_hats;
                }
            }
        }

        //3 Axis
        {
            bool write;

            write = m_portAxes.getOutputCount()>0;
            auto& b = m_portAxes.prepare();
            b.value.clear();
            for(size_t i = 0; i < m_numberOfAxes; ++i)
            {
                if (!m_IJoypad->getAxis(i, m_curr_axes[i]))
                {
                    write = false;
                    break;
                }
            }

            if (write)
            {
                if (m_publish_on_event)
                {
                   if (VectorsDifferent(m_curr_axes,m_last_axes))
                   {
                      b.value = m_curr_axes;
                      m_portAxes.write();
                      m_last_axes = m_curr_axes;
                   }
                }
                else
                {
                   b.value = m_curr_axes;
                   m_portAxes.write();
                   m_last_axes = m_curr_axes;
                }
            }
        }

        //4 Trackball
        {
            bool write;

            write     = m_portTrackballs.getOutputCount()>0;
            auto& b = m_portTrackballs.prepare();
            b.value.clear();
            for(size_t i = 0; i < m_numberOfTrackballs; ++i)
            {
                if (!m_IJoypad->getTrackball(i, m_curr_trackballs[i]))
                {
                    write = false;
                    break;
                }
            }

            if (write)
            {
                if (m_publish_on_event)
                {
                   if (VectorsDifferentC(m_curr_trackballs,m_last_trackballs,
                       [](const yarp::dev::TrackballData& lhs,
                          const yarp::dev::TrackballData& rhs)
                          {
                              return lhs.x == rhs.x &&
                                     lhs.y == rhs.y;
                          }
                       ))
                   {
                      b.value = m_curr_trackballs;
                      m_portTrackballs.write();
                      m_last_trackballs = m_curr_trackballs;
                   }
                }
                else
                {
                   b.value = m_curr_trackballs;
                   m_portTrackballs.write();
                   m_last_trackballs = m_curr_trackballs;
                }
            }
        }

        //5 Stick
        {
            bool write;
            write = m_portSticks.getOutputCount()>0;
            auto& b = m_portSticks.prepare();
            b.value.clear();
            for(size_t i = 0; i < m_numberOfSticks; ++i)
            {
                if (!m_IJoypad->getStick(i, m_curr_sticks[i], m_coordsMode))
                {
                    write = false;
                    break;
                }
            }

            if (write)
            {
                if (m_publish_on_event)
                {
                   if (VectorsDifferentC(m_curr_sticks,m_last_sticks,
                       [](const yarp::dev::StickData& lhs,
                          const yarp::dev::StickData& rhs)
                          {
                              return lhs.s1 == rhs.s1 &&
                                     lhs.s2 == rhs.s2;
                          }
                       ))
                   {
                      b.value = m_curr_sticks;
                      m_portSticks.write();
                      m_last_sticks = m_curr_sticks;
                   }
                }
                else
                {
                   b.value = m_curr_sticks;
                   m_portSticks.write();
                   m_last_sticks = m_curr_sticks;
                }
            }
        }

        //6 Touch
        {
            bool write;
            write =  m_portTouches.getOutputCount()>0;
            auto& b = m_portTouches.prepare();
            b.value.clear();

            m_curr_touches.value.resize(m_numberOfTouchSurfaces);
            for(unsigned int i = 0; i < m_numberOfTouchSurfaces; ++i)
            {
                std::vector<yarp::dev::TouchData> temp;
                if (!m_IJoypad->getTouch(i, temp))
                {
                    write = false;
                    break;
                }
                m_curr_touches.value[i].touches = temp;
            }

            if (write)
            {
                if (m_publish_on_event)
                {
                   if (1)
                   //if (VectorsDifferent(m_curr_touches.value,m_last_touches.value))
                   {
                      b.value = m_curr_touches.value;
                      m_portTouches.write();
                      m_last_touches = m_curr_touches;
                   }
                }
                else
                {
                   b.value = m_curr_touches.value;
                   m_portTouches.write();
                   m_last_touches = m_curr_touches;
                }
            }
        }
    }
    else
    {
        //JoyData& message = m_allPort.prepare();
        yCError(JOYPADCONTROLSERVER) << "Not implemented yet";
    }

    //print data on screen
    if(m_profile)
    {
        profile_message =  " Axes: ";
        profile_message += printVector(m_curr_axes);
        profile_message += " Buttons: ";
        profile_message += printVector(m_curr_buttons);
        //profile_message += " Trackballs: ";
        //profile_message += printVector(m_curr_trackballs);
        //profile_message += " Sticks: ";
        //profile_message += printVector(m_curr_sticks);
        //profile_message += " Hats: ";
        //profile_message += printVector(m_curr_hats);
        //profile_message += " Touch Surface: ";
        //profile_message += printVector(m_curr_touches);
        yCInfo(JOYPADCONTROLSERVER) << profile_message;
    }
}

bool JoypadControl_nws_yarp::close()
{
    detachAll();

    // Closing port
    m_portAxes.close();
    m_portButtons.close();
    m_portSticks.close();
    m_portTouches.close();
    m_portTrackballs.close();
    m_portHats.close();
    m_rpcPort.close();

    return true;
}

bool JoypadControl_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    if (!connection.isValid()) { return false;}
    if (!m_RPCMsgs) { return false;}

    if (m_RPCMsgs)
    {
        bool b = m_RPCMsgs->read(connection);
        if (b) {
            return true;
        }
    }
    yCDebug(JOYPADCONTROLSERVER) << "read() Command failed";
    return false;
}

#include "JoypadControlClient.h"
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;


bool JoypadControlClient::getAxisCount(unsigned int& axis_count)
{
    return getCount(VOCAB_AXIS, axis_count);
}

bool JoypadControlClient::open(yarp::os::Searchable& config)
{
    if (!config.check("period", "refresh period of the broadcasted values in ms"))
    {
        yInfo() << "JoypadControlServer: using default 'period' parameter of " << DEFAULT_THREAD_PERIOD << "ms";
    }
    else
    {
        m_rate = config.find("period").asInt();
    }
    if(config.check("local"))
    {
        m_local = config.find("local").asString();
        if(m_rpcPort.open(m_local + "/rpc:o"))
        {
            yInfo() << "rpc port opened.. starting the handshake";
            if(config.check("remote"))
            {
                m_remoteRpc = config.find("remote").asString();
                if(yarp::os::NetworkBase::connect(m_local + "/rpc:o", m_remoteRpc))
                {
                    yInfo() << "handshake succeded! retrieving info";
                }
                else
                {
                    yError() << "handshake failed.. unable to connect to remote port..";
                }
            }
            else
            {
                yError() << "JoypadControlClient: unable to find the 'remote' parameter.. check configuration file";
            }
        }
        else
        {
            yError() << "JoypadControlClient: unable to open rpc port..";
        }
    }
    else
    {
        yError() << "JoypadControlClient: unable to 'local' parameter.. check configuration file";
    }
    return true;
}

void JoypadControlClient::run()
{
    LockGuard l(m_mutex);
    Vector* data = YARP_NULLPTR;
    data = m_buttonsPort.read();
    if(data)
    {
        m_buttons = *data;
    }
}

bool JoypadControlClient::getCount(const int& vocab_toget, unsigned int& value)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_IJOYPADCTRL);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(vocab_toget);
    cmd.addVocab(VOCAB_COUNT);
    m_rpcPort.write(cmd, response);
    if(response.get(0).asVocab() == VOCAB_OK && response .get(1).isInt())
    {
        value = response.get(1).asInt();
        return true;
    }
    else
    {
        return false;
    }
}

bool JoypadControlClient::getButtonCount(unsigned int& button_count)
{
    return getCount(VOCAB_BUTTON, button_count);
}
bool JoypadControlClient::getTrackballCount(unsigned int& Trackball_count)
{
    return getCount(VOCAB_TRACKBALL, Trackball_count);
}
bool JoypadControlClient::getHatCount(unsigned int& Hat_count)
{
    return getCount(VOCAB_HAT, Hat_count);
}
bool JoypadControlClient::getTouchSurfaceCount(unsigned int& touch_count)
{
    return getCount(VOCAB_TOUCH, touch_count);
}
bool JoypadControlClient::getStickCount(unsigned int& stick_count)
{
    return getCount(VOCAB_STICK, stick_count);;
}
bool JoypadControlClient::getStickDoF(unsigned int stick_id, unsigned int& DoF)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_IJOYPADCTRL);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_STICKDOF);
    cmd.addVocab(VOCAB_COUNT);
    cmd.addInt(stick_id);
    m_rpcPort.write(cmd, response);
    if(response.get(0).asVocab() == VOCAB_OK && response .get(1).isInt())
    {
        DoF = response.get(1).asInt();
        return true;
    }
    else
    {
        return false;
    }
}

bool JoypadControlClient::getButton(unsigned int button_id, float& value)
{
    if(m_rpc_only)
    {
        Bottle cmd, response;

        cmd.addVocab(VOCAB_IJOYPADCTRL);
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_BUTTON);
        cmd.addVocab(VOCAB_VALUE);
        cmd.addInt(button_id);
        m_rpcPort.write(cmd, response);
        if(response.get(0).asVocab() == VOCAB_OK && response .get(1).isDouble())
        {
            value = response.get(1).asDouble();
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        LockGuard l(m_mutex);
        if(button_id < m_buttons.size())
        {
            value = m_buttons[button_id];
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool JoypadControlClient::getTrackball(unsigned int trackball_id, yarp::sig::Vector& value)
{
    if(m_rpc_only)
    {
        Bottle cmd, response;

        cmd.addVocab(VOCAB_IJOYPADCTRL);
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_TRACKBALL);
        cmd.addVocab(VOCAB_VALUE);
        cmd.addInt(trackball_id);
        m_rpcPort.write(cmd, response);
        if(response.get(0).asVocab() == VOCAB_OK && response .get(1).isDouble() && response.get(2).isDouble())
        {
            value.push_back(response.get(1).asDouble());
            value.push_back(response.get(2).asDouble());
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        LockGuard l(m_mutex);
        if(trackball_id < m_trackballs.size()/2)
        {
            value.push_back(m_trackballs[trackball_id*2]);
            value.push_back(m_trackballs[trackball_id*2 + 1]);
            return true;
        }
        else
        {
            return false;
        }
    }
}
bool JoypadControlClient::getHat(unsigned int hat_id, unsigned char& value)
{
    if(m_rpc_only)
    {
        Bottle cmd, response;

        cmd.addVocab(VOCAB_IJOYPADCTRL);
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_HAT);
        cmd.addVocab(VOCAB_VALUE);
        cmd.addInt(hat_id);
        m_rpcPort.write(cmd, response);
        if(response.get(0).asVocab() == VOCAB_OK && response .get(1).isInt())
        {
            value = response.get(1).asInt();
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        LockGuard l(m_mutex);
        if(hat_id < m_hats.size())
        {
            value = m_hats[hat_id];
            return true;
        }
        else
        {
            return false;
        }
    }
}
bool JoypadControlClient::getAxis(unsigned int axis_id, double& value)
{
    if(m_rpc_only)
    {
        Bottle cmd, response;

        cmd.addVocab(VOCAB_IJOYPADCTRL);
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_AXIS);
        cmd.addVocab(VOCAB_VALUE);
        cmd.addInt(axis_id);
        m_rpcPort.write(cmd, response);
        if(response.get(0).asVocab() == VOCAB_OK && response .get(1).isDouble())
        {
            value = response.get(1).asDouble();
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        LockGuard l(m_mutex);
        if(axis_id < m_axis.size())
        {
            value = m_axis[axis_id];
            return true;
        }
        else
        {
            return false;
        }
    }
}
bool JoypadControlClient::getStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode)
{
    if(m_rpc_only)
    {
        Bottle cmd, response;
        int    dof, coordmode;

        coordmode = coordinate_mode == IJoypadController::JypCtrlcoord_CARTESIAN ? VOCAB_CARTESIAN : VOCAB_POLAR;
        cmd.addVocab(VOCAB_IJOYPADCTRL);
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_STICKDOF);
        cmd.addVocab(VOCAB_VALUE);
        cmd.addInt(stick_id);
        m_rpcPort.write(cmd, response);

        if(response.get(0).asVocab() == VOCAB_OK && response.get(1).isInt())
        {
            dof = response.get(1).asInt();
        }
        else
        {
            return false;
        }
        cmd.addVocab(VOCAB_IJOYPADCTRL);
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_STICK);
        cmd.addVocab(VOCAB_VALUE);
        cmd.addVocab(coordmode);
        cmd.addInt(stick_id);
        m_rpcPort.write(cmd, response);
        if(response.get(0).asVocab() == VOCAB_OK)
        {
            for(int i = 0; i < dof; i++)
            {
                if(response.get(i).isDouble())
                {
                    value.push_back(response.get(i).asDouble());
                }
                else
                {
                    return false;
                }
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        LockGuard l(m_mutex);
        if(stick_id < m_sticks.size())
        {
            value = m_sticks[stick_id];
            return true;
        }
        else
        {
            return false;
        }
    }
}
bool JoypadControlClient::getTouch(unsigned int touch_id, yarp::sig::Vector& value)
{
    if(m_rpc_only)
    {
        Bottle cmd, response;

        cmd.addVocab(VOCAB_IJOYPADCTRL);
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(VOCAB_TOUCH);
        cmd.addVocab(VOCAB_VALUE);
        cmd.addInt(touch_id);
        m_rpcPort.write(cmd, response);
        if(response.get(0).asVocab() == VOCAB_OK && response .get(1).isDouble() && response.get(2).isDouble())
        {
            value.push_back(response.get(1).asDouble());
            value.push_back(response.get(2).asDouble());
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        LockGuard l(m_mutex);
        if(touch_id < m_touch.size()/2)
        {
            value.push_back(m_touch[touch_id*2]);
            value.push_back(m_touch[touch_id*2 + 1]);
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool JoypadControlClient::close()
{
    return true;
}

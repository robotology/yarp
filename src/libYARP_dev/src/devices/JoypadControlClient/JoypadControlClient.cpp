/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "JoypadControlClient.h"
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>
#include <tuple>
#include <yarp/dev/DriverLinkCreator.h>
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;
using namespace std;

// needed for the driver factory.
yarp::dev::DriverCreator* createJoypadControlClient()
{
    return new DriverCreatorOf<yarp::dev::JoypadControlClient>("JoypadControlClient", "JoypadControlClient", "yarp::dev::JoypadControlClient");
}

JoypadControlClient::JoypadControlClient() :  m_rpc_only(false)
{
    m_ports.push_back(&m_buttonsPort  );
    m_ports.push_back(&m_axisPort     );
    m_ports.push_back(&m_stickPort    );
    m_ports.push_back(&m_trackballPort);
    m_ports.push_back(&m_touchPort    );
    m_ports.push_back(&m_hatsPort     );
    watchdog.m_ports = m_ports;
}

bool JoypadControlClient::getJoypadInfo()
{
    unsigned int count;
    bool         temp;

    temp       = m_rpc_only;
    m_rpc_only = true;
    vector<tuple<int, JoypadControl::LoopablePort*, string> > vocabs_ports;
    vocabs_ports.push_back(make_tuple(VOCAB_BUTTON,    &m_buttonsPort,   "/buttons"));
    vocabs_ports.push_back(make_tuple(VOCAB_AXIS,      &m_axisPort,      "/axis"));
    vocabs_ports.push_back(make_tuple(VOCAB_STICK,     &m_stickPort,     "/stick"));
    vocabs_ports.push_back(make_tuple(VOCAB_TRACKBALL, &m_trackballPort, "/trackballs"));
    vocabs_ports.push_back(make_tuple(VOCAB_TOUCH,     &m_touchPort,     "/touch"));
    vocabs_ports.push_back(make_tuple(VOCAB_HAT,       &m_hatsPort,      "/hat"));

    for(auto vocab_port : vocabs_ports)
    {
        if(!getCount(get<0>(vocab_port), count)) return false;
        if(count)
        {
            string source;
            string destination;
            std::string portname = m_local + get<2>(vocab_port) + ":i";
            get<1>(vocab_port)->valid = true;
            get<1>(vocab_port)->count = count;

            if(get<0>(vocab_port) == VOCAB_STICK)
            {
                for(unsigned int i = 0; i < count; i++)
                {
                    unsigned int dofCount;
                    if(!getStickDoF(i, dofCount))
                    {
                        yError() << "unable to get sticks DoF";
                        return false;
                    }
                    m_stickDof.push_back(dofCount);
                }
            }

            yInfo() << "opening" << portname;

            if(!get<1>(vocab_port)->contactable->open(portname))
            {
                yError() << "unable to open" << portname << "port";
                return false;
            }

            source      = m_remote + get<2>(vocab_port) + ":o";
            destination = m_local  + get<2>(vocab_port) + ":i";
            if(!yarp::os::NetworkBase::connect(source.c_str(), destination.c_str(), "udp"))
            {
                yError() << "unable to connect" << portname << "port";
                return false;
            }

            get<1>(vocab_port)->useCallback();
        }
    }
    m_rpc_only = temp;
    return true;
}

bool JoypadControlClient::open(yarp::os::Searchable& config)
{
    if(config.check("help"))
    {
        yInfo() << "parameter:\n\n" <<
                   "local  - prefix of the local port\n" <<
                   "remote - prefix of the port provided to and opened by JoypadControlServer\n";
    }
    if(!config.check("local"))
    {
        yError() << "JoypadControlClient: unable to 'local' parameter.. check configuration file";
        return false;
    }

    m_local = config.find("local").asString();

    if(!m_rpcPort.open(m_local + "/rpc:o"))
    {
        yError() << "JoypadControlClient: unable to open rpc port..";
        return false;
    }

    yInfo() << "rpc port opened.. starting the handshake";

    if(!config.check("remote"))
    {
        yError() << "JoypadControlClient: unable to find the 'remote' parameter.. check configuration file";
        return false;
    }

    m_remote = config.find("remote").asString();

    if(!yarp::os::NetworkBase::connect(m_local + "/rpc:o", m_remote + "/rpc:i"))
    {
        yError() << "handshake failed.. unable to connect to remote port" << m_remote + "/rpc:i";
        return false;
    }

    yInfo() << "handshake succeded! retrieving info";

    if(!getJoypadInfo())
    {
        yError() << "unable to get joypad info..";
        return false;
    }

    watchdog.start();

    return true;
}

void JoypadControlWatchdog::run()
{
    for (auto port : m_ports)
    {
        if (port->count)
        {
            port->onTimeout(0.5);
        }
    }
}

bool JoypadControlClient::getCount(const int& vocab_toget, unsigned int& value)
{
    if(!m_rpc_only)
    {
        switch(vocab_toget)
        {
        case VOCAB_BUTTON:
            value = m_buttonsPort.count;
            return true;

        case VOCAB_AXIS:

            value = m_axisPort.count;
            return true;

        case VOCAB_TRACKBALL:
            value = m_trackballPort.count;
            return true;

        case VOCAB_TOUCH:

            value = m_touchPort.count;
            return true;

        case VOCAB_STICK:

            value = m_stickPort.count;
            return true;

        case VOCAB_HAT:

            value = m_hatsPort.count;
            return true;

        default:
            break;
        }
    }

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

bool JoypadControlClient::getRawButtonCount(unsigned int& button_count)
{
    return getCount(VOCAB_BUTTON, button_count);
}

bool JoypadControlClient::getRawAxisCount(unsigned int& axis_count)
{
    return getCount(VOCAB_AXIS, axis_count);
}

bool JoypadControlClient::getRawTrackballCount(unsigned int& Trackball_count)
{
    return getCount(VOCAB_TRACKBALL, Trackball_count);
}

bool JoypadControlClient::getRawHatCount(unsigned int& Hat_count)
{
    return getCount(VOCAB_HAT, Hat_count);
}

bool JoypadControlClient::getRawTouchSurfaceCount(unsigned int& touch_count)
{
    return getCount(VOCAB_TOUCH, touch_count);
}

bool JoypadControlClient::getRawStickCount(unsigned int& stick_count)
{
    return getCount(VOCAB_STICK, stick_count);
}

bool JoypadControlClient::getRawStickDoF(unsigned int stick_id, unsigned int& DoF)
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

bool JoypadControlClient::getRawButton(unsigned int button_id, float& value)
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
            yError() << "JoypadControlCLient: GetButton() error.. VOCAB_FAILED";
            return false;
        }
    }
    else
    {
        LockGuard l(m_buttonsPort.mutex);
        if(button_id < m_buttonsPort.storage.size())
        {
            value = m_buttonsPort.storage[button_id];
            return true;
        }
        else
        {
            yError() << "JoypadControlCLient: GetButton() error.. button_id out of bound";
            return false;
        }
    }
}

bool JoypadControlClient::getRawTrackball(unsigned int trackball_id, yarp::sig::Vector& value)
{
    value.clear();
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
        LockGuard l(m_trackballPort.mutex);
        if(trackball_id < m_trackballPort.storage.size() / 2)
        {
            value.push_back(m_trackballPort.storage[trackball_id * 2]);
            value.push_back(m_trackballPort.storage[trackball_id * 2 + 1]);
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool JoypadControlClient::getRawHat(unsigned int hat_id, unsigned char& value)
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
        LockGuard l(m_hatsPort.mutex);
        if(hat_id < m_hatsPort.storage.size())
        {
            value = m_hatsPort.storage[hat_id];
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool JoypadControlClient::getRawAxis(unsigned int axis_id, double& value)
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
            yError() << "JoypadControlCLient: GetAxis() error.. VOCAB_FAILED";
            return false;
        }
    }
    else
    {
        LockGuard l(m_axisPort.mutex);
        if(axis_id < m_axisPort.storage.size())
        {
            value = m_axisPort.storage[axis_id];
            return true;
        }
        else
        {
            yError() << "JoypadControlCLient: GetAxis() error.. Axis_id out of bound";
            return false;
        }
    }
}

bool JoypadControlClient::getRawStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode)
{
    value.clear();
    if(m_rpc_only || coordinate_mode == IJoypadController::JypCtrlcoord_POLAR)
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
        LockGuard l(m_stickPort.mutex);
        int offset = 0;

        unsigned int i;
        if(getStickCount(i), stick_id >= i)
        {
            yError() << "JoypadControlCLient: GetStick() error.. Stick_id out of bound";
            return false;
        }
        for(size_t j = 0; j < stick_id; j++)
        {
            offset += m_stickDof[j];
        }

        for(size_t i = 0; i < m_stickDof[stick_id]; ++i)
        {
            value.push_back(m_stickPort.storage[offset + i]);
        }

        return true;
    }
}

bool JoypadControlClient::getRawTouch(unsigned int touch_id, yarp::sig::Vector& value)
{
    value.clear();
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
        LockGuard l(m_touchPort.mutex);
        if(touch_id < m_touchPort.storage.size()/2)
        {
            value.push_back(m_touchPort.storage[touch_id * 2]);
            value.push_back(m_touchPort.storage[touch_id * 2 + 1]);
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

    vector<JoypadControl::LoopablePort*> portv;
    portv.push_back(&m_buttonsPort);
    portv.push_back(&m_axisPort);
    portv.push_back(&m_hatsPort);
    portv.push_back(&m_touchPort);
    portv.push_back(&m_trackballPort);
    portv.push_back(&m_stickPort);

    for(auto p : portv)
    {
        p->contactable->interrupt();
        p->contactable->close();
    }

    m_rpcPort.close();
    return true;
}

/*
 * Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#define _USE_MATH_DEFINES

#include "audioPlayerWrapper.h"
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/LogStream.h>

#include <cmath>
#include <sstream>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;


audioPlayerWrapper::audioPlayerWrapper() : PeriodicThread(DEFAULT_THREAD_PERIOD),
    m_irender(nullptr),
    m_period(DEFAULT_THREAD_PERIOD),
    m_isDeviceOwned(false)
{}

audioPlayerWrapper::~audioPlayerWrapper()
{
    m_irender = nullptr;
}

/**
  * Specify which sensor this thread has to read from.
  */

bool audioPlayerWrapper::attachAll(const PolyDriverList &device2attach)
{
    if (device2attach.size() != 1)
    {
        yError("audioPlayerWrapper: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(m_irender);
    }

    if (nullptr == m_irender)
    {
        yError("audioPlayerWrapper: subdevice passed to attach method is invalid");
        return false;
    }
    attach(m_irender);

    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}

bool audioPlayerWrapper::detachAll()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    m_irender = nullptr;
    return true;
}

void audioPlayerWrapper::attach(yarp::dev::IAudioRender *irend)
{
    m_irender = irend;
}

void audioPlayerWrapper::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    m_irender = nullptr;
}

bool audioPlayerWrapper::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    // parse in, prepare out
    int action = in.get(0).asVocab();
    int inter  = in.get(1).asVocab();
    bool ret = false;

/*    if (inter == VOCAB_ILASER2D)
    {
        if (action == VOCAB_GET)
        {

        }
        else if (action == VOCAB_SET)
        {

        }
        else
        {
            yError("Invalid action received in audioPlayerWrapper");
        }
    }
    else
    {
        yError("Invalid interface vocab received in audioPlayerWrapper");
    }
    */

    if (!ret)
    {
        out.clear();
        out.addVocab(VOCAB_FAILED);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr) 
    {
        out.write(*returnToSender);
    }
    return true;
}

bool audioPlayerWrapper::threadInit()
{
    return true;
}

bool audioPlayerWrapper::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString());

    if (config.check("period"))
    {
        m_period = config.find("period").asFloat64();
    }

    string name = "/audioPlayerWrapper";
    if (config.check("name"))
    {
        name = config.find("name").asString();
    }
    m_audioInPortName = name + "/audio:i";
    m_rpcPortName = name + "/rpc:i";
    m_statusPortName = name + "/status:o";
 
    if(!initialize_YARP(config) )
    {
        yError() << "audioPlayerWrapper: Error initializing YARP ports";
        return false;
    }

    if(config.check("subdevice"))
    {
        Property       p;
        PolyDriverList driverlist;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if(!m_driver.open(p) || !m_driver.isValid())
        {
            yError() << "audioPlayerWrapper: failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&m_driver, "1");
        if(!attachAll(driverlist))
        {
            yError() << "audioPlayerWrapper: failed to open subdevice.. check params";
            return false;
        }
        m_isDeviceOwned = true;
    }


    if (m_irender == nullptr)
    {
        yError("m_irender is null\n");
        return false;
    }

    bool b=m_irender->getPlaybackAudioBufferMaxSize(m_max_buffer_size);
    if (!b)
    {
        yError("getPlaybackAudioBufferMaxSize failed\n");
        return false;
    }

    return true;
}

bool audioPlayerWrapper::initialize_YARP(yarp::os::Searchable &params)
{
    if (!m_audioInPort.open(m_audioInPortName))
    {
        yError("audioPlayerWrapper: failed to open port %s", m_audioInPortName.c_str());
        return false;
    }
    if (!m_statusPort.open(m_statusPortName))
    {
        yError("audioPlayerWrapper: failed to open port %s", m_statusPortName.c_str());
        return false;
    }
    if (!m_rpcPort.open(m_rpcPortName))
    {
        yError("audioPlayerWrapper: failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);
    return true;
}

void audioPlayerWrapper::threadRelease()
{
    m_audioInPort.interrupt();
    m_audioInPort.close();
    m_rpcPort.interrupt();
    m_rpcPort.close();
    m_statusPort.interrupt();
    m_statusPort.close();
}

void audioPlayerWrapper::run()
{
    Sound *s=nullptr;

    s = m_audioInPort.read(false);
    if (s != NULL)
    {
        m_irender->renderSound(*s);
    }

    m_irender->getPlaybackAudioBufferCurrentSize(m_current_buffer_size);
#if 1
    static size_t count = 0;
    if (count>100)
    {
        yDebug() << m_current_buffer_size.getSamples() << "samples";
        count= 0;
    }
    count++;
#endif

}

bool audioPlayerWrapper::close()
{
    yTrace("audioPlayerWrapper::Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    detachAll();
    return true;
}

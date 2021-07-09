/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RemoteControlBoardRemapper.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <cassert>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace std;

namespace {
YARP_LOG_COMPONENT(REMOTECONTROLBOARDREMAPPER, "yarp.device.remotecontrolboardremapper")
}


void RemoteControlBoardRemapper::closeAllRemoteControlBoards()
{
    for(auto& m_remoteControlBoardDevice : m_remoteControlBoardDevices)
    {
        if( m_remoteControlBoardDevice )
        {
            m_remoteControlBoardDevice->close();
            delete m_remoteControlBoardDevice;
            m_remoteControlBoardDevice = nullptr;
        }
    }

    m_remoteControlBoardDevices.resize(0);
}


bool RemoteControlBoardRemapper::close()
{
    bool ret = true;

    bool ok = ControlBoardRemapper::detachAll();

    ret = ret && ok;

    ok = ControlBoardRemapper::close();

    ret = ret && ok;

    closeAllRemoteControlBoards();

    return ret;
}

bool RemoteControlBoardRemapper::open(Searchable& config)
{
    Property prop;
    prop.fromString(config.toString());

    std::string localPortPrefix;
    std::vector<std::string> remoteControlBoardsPorts;

    // Check if the required parameters  are found
    if( prop.check("localPortPrefix") && prop.find("localPortPrefix").isString() )
    {
        localPortPrefix = prop.find("localPortPrefix").asString();
    }
    else
    {
        yCError(REMOTECONTROLBOARDREMAPPER) << "Parsing parameters: \"localPortPrefix\" should be a string.";
        return false;
    }

    Bottle *remoteControlBoards=prop.find("remoteControlBoards").asList();
    if(remoteControlBoards==nullptr)
    {
        yCError(REMOTECONTROLBOARDREMAPPER) << "Parsing parameters: \"remoteControlBoards\" should be followed by a list.";
        return false;
    }

    remoteControlBoardsPorts.resize(remoteControlBoards->size());
    for(size_t ax=0; ax < remoteControlBoards->size(); ax++)
    {
        remoteControlBoardsPorts[ax] = remoteControlBoards->get(ax).asString();
    }

    // Load the REMOTE_CONTROLBOARD_OPTIONS, containing any additional option to pass to the remote control boards
    Property remoteControlBoardsOptions;

    Bottle & optionsGroupBot = prop.findGroup("REMOTE_CONTROLBOARD_OPTIONS");
    if( !(optionsGroupBot.isNull()) )
    {
        remoteControlBoardsOptions.fromString(optionsGroupBot.toString());
    }

    // Parameters loaded, open all the remote controlboards

    m_remoteControlBoardDevices.resize(remoteControlBoardsPorts.size(),nullptr);

    PolyDriverList remoteControlBoardsList;

    for(size_t ctrlBrd=0; ctrlBrd < remoteControlBoardsPorts.size(); ctrlBrd++ )
    {
        std::string remote = remoteControlBoardsPorts[ctrlBrd];
        // Note: as local parameter we use localPortPrefix+remoteOfTheReportControlBoard
        std::string local = localPortPrefix+remote;

        Property options = remoteControlBoardsOptions;
        options.put("device", "remote_controlboard");
        options.put("local", local);
        options.put("remote", remote);

        m_remoteControlBoardDevices[ctrlBrd] = new PolyDriver();

        bool ok = m_remoteControlBoardDevices[ctrlBrd]->open(options);

        if( !ok || !(m_remoteControlBoardDevices[ctrlBrd]->isValid()) )
        {
            yCError(REMOTECONTROLBOARDREMAPPER) << "Opening remote_controlboard with remote \"" << remote << "\", opening the device failed.";
            closeAllRemoteControlBoards();
            return false;
        }

        // We use the remote name of the remote_controlboard as the key for it, in absence of anything better
        remoteControlBoardsList.push((m_remoteControlBoardDevices[ctrlBrd]),remote.c_str());
    }

    // Device opened, now we open the ControlBoardRemapper and then we call attachAll
    bool ok = ControlBoardRemapper::open(prop);

    if( !ok )
    {
        yCError(REMOTECONTROLBOARDREMAPPER) << "Opening the controlboardremapper device, opening the device failed.";
        ControlBoardRemapper::close();
        closeAllRemoteControlBoards();
        return false;
    }

    // If open went ok, we now call attachAll
    ok = ControlBoardRemapper::attachAll(remoteControlBoardsList);

    if( !ok )
    {
        yCError(REMOTECONTROLBOARDREMAPPER) << "Calling attachAll in the controlboardremapper device, opening the device failed.";
        ControlBoardRemapper::close();
        closeAllRemoteControlBoards();
        return false;
    }

    // All went ok, return true
    // TODO: close devices that are not actually used by the remapper
    return true;
}

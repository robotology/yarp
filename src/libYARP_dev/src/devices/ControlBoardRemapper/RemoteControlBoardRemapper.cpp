/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "RemoteControlBoardRemapper.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <cassert>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace std;

// needed for the driver factory.
DriverCreator *createRemoteControlBoardRemapper()
{
    return new DriverCreatorOf<yarp::dev::RemoteControlBoardRemapper>
            ("remotecontrolboardremapper", "controlboardwrapper2", "yarp::dev::RemoteControlBoardRemapper");
}

RemoteControlBoardRemapper::RemoteControlBoardRemapper()
{
}

RemoteControlBoardRemapper::~RemoteControlBoardRemapper()
{
}

void RemoteControlBoardRemapper::closeAllRemoteControlBoards()
{
    for(size_t ctrlBrd=0; ctrlBrd < m_remoteControlBoardDevices.size(); ctrlBrd++)
    {
        if( m_remoteControlBoardDevices[ctrlBrd] )
        {
            m_remoteControlBoardDevices[ctrlBrd]->close();
            delete m_remoteControlBoardDevices[ctrlBrd];
            m_remoteControlBoardDevices[ctrlBrd] = nullptr;
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
    prop.fromString(config.toString().c_str());

    std::string localPortPrefix;
    std::vector<std::string> remoteControlBoardsPorts;

    // Check if the required parameters  are found
    if( prop.check("localPortPrefix") && prop.find("localPortPrefix").isString() )
    {
        localPortPrefix = prop.find("localPortPrefix").asString().c_str();
    }
    else
    {
        yError() <<"RemoteControlBoardRemapper: Error parsing parameters: \"localPortPrefix\" should be a string.";
        return false;
    }

    Bottle *remoteControlBoards=prop.find("remoteControlBoards").asList();
    if(remoteControlBoards==nullptr)
    {
        yError() <<"RemoteControlBoardRemapper: Error parsing parameters: \"remoteControlBoards\" should be followed by a list.";
        return false;
    }

    remoteControlBoardsPorts.resize(remoteControlBoards->size());
    for(int ax=0; ax < remoteControlBoards->size(); ax++)
    {
        remoteControlBoardsPorts[ax] = remoteControlBoards->get(ax).asString().c_str();
    }

    // Load the REMOTE_CONTROLBOARD_OPTIONS, containg any additional option to pass to the remote control boards
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
            yError() << "RemoteControlBoardRemapper: error opening remote_controlboard with remote \"" << remote << "\", opening the device failed.";
            closeAllRemoteControlBoards();
            return false;
        }

        // We use the remote name of the remote_controlboard as the key for it, in absense of anything better
        remoteControlBoardsList.push((m_remoteControlBoardDevices[ctrlBrd]),remote.c_str());
    }

    // Device opened, now we open the ControlBoardRemapper and then we call attachAll
    bool ok = ControlBoardRemapper::open(prop);

    if( !ok )
    {
        yError() << "RemoteControlBoardRemapper: error opening the controlboardremapper device, opening the device failed.";
        ControlBoardRemapper::close();
        closeAllRemoteControlBoards();
        return false;
    }

    // If open went ok, we now call attachAll
    ok = ControlBoardRemapper::attachAll(remoteControlBoardsList);

    if( !ok )
    {
        yError() << "RemoteControlBoardRemapper: error calling attachAll in the controlboardremapper device, opening the device failed.";
        ControlBoardRemapper::close();
        closeAllRemoteControlBoards();
        return false;
    }

    // All went ok, return true
    // TODO: close devices that are not actually used by the remapper
    return true;
}


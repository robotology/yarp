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

namespace {
YARP_LOG_COMPONENT(REMOTECONTROLBOARDREMAPPER, "yarp.device.remotecontrolboardremapper")
}


void RemoteControlBoardRemapper::closeAllRemoteControlBoards()
{
    for(size_t i = 0; i<m_remoteControlBoardDevices.size(); i++)
    {
        if (m_remoteControlBoardDevices[i])
        {
            m_remoteControlBoardDevices[i]->poly->close();
            delete m_remoteControlBoardDevices[i]->poly;
            m_remoteControlBoardDevices[i]->key = "";
            m_remoteControlBoardDevices[i]->poly = nullptr;
        }
    }

    m_remoteControlBoardDevices.clear();
}


bool RemoteControlBoardRemapper::close()
{
    bool ret = ControlBoardRemappingEngine::detachAllDevices();

    closeAllRemoteControlBoards();

    return ret;
}

bool RemoteControlBoardRemapper::openAllRemoteControlBoards(const Property& remoteControlBoardsOptions)
{
    for(size_t ctrlBrd=0; ctrlBrd < m_remoteControlBoards.size(); ctrlBrd++ )
    {
        std::string remote = m_remoteControlBoards[ctrlBrd];
        // Note: as local parameter we use localPortPrefix+remoteOfTheReportControlBoard
        std::string local = m_localPortPrefix+remote;

        Property options = remoteControlBoardsOptions;
        options.put("device", "controlBoard_nwc_yarp");
        options.put("local", local);
        options.put("remote", remote);

        //the pointer will be inserted in a PolyDriverList, that will take care of closing the PolyDriver when it is destroyed
        PolyDriver* tmpDriver = new PolyDriver;
        bool ok = tmpDriver->open(options);

        if( !ok || !(tmpDriver->isValid()) )
        {
            yCError(REMOTECONTROLBOARDREMAPPER) << "Opening controlBoard_nwc_yarp with remote \"" << remote << "\", opening the device failed.";
            closeAllRemoteControlBoards();
            return false;
        }

        // We use the remote name of the controlBoard_nwc_yarp as the key for it, in absence of anything better
        m_remoteControlBoardDevices.push(tmpDriver,remote.c_str());
    }

    return true;
}

bool RemoteControlBoardRemapper::open(Searchable& config)
{
    // From RemoteControlBoardRemapper_ParamsParser
    bool b = parseParams(config);
    if (!b) return false;

    // Load the REMOTE_CONTROLBOARD_OPTIONS, containing any additional option to pass to the remote control boards
    Property prop;
    prop.fromString(config.toString());
    Property remoteControlBoardsOptions;
    Bottle & optionsGroupBot = prop.findGroup("REMOTE_CONTROLBOARD_OPTIONS");
    if( !(optionsGroupBot.isNull()) )
    {
        remoteControlBoardsOptions.fromString(optionsGroupBot.toString());
    }

    // Open all the remote control boards
    b = openAllRemoteControlBoards(remoteControlBoardsOptions);
    if (!b) return false;

    // Device opened, now we open the ControlBoardRemapper and then we call attachAll
    bool ok = true;
/*  ok = ControlBoardRemappingEngine::open(prop);
    if( !ok )
    {
        yCError(REMOTECONTROLBOARDREMAPPER) << "Opening the controlboardremapper device, opening the device failed.";
        ControlBoardRemappingEngine::detachAllDevices();
        closeAllRemoteControlBoards();
        return false;
    }
*/
    this->setAxesNames(m_axesNames);
    this->setNrOfControlledAxes(m_axesNames.size());

    // If open went ok, we now call attachAll
    ok = ControlBoardRemappingEngine::attachAllDevices(m_remoteControlBoardDevices);

    if( !ok )
    {
        yCError(REMOTECONTROLBOARDREMAPPER) << "Calling attachAll in the controlboardremapper device, opening the device failed.";
        ControlBoardRemappingEngine::detachAllDevices();
        closeAllRemoteControlBoards();
        return false;
    }

    // All went ok, return true
    // TODO: close devices that are not actually used by the remapper
    return true;
}

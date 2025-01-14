/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "MultipleAnalogSensorsClientRemapper.h"

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
YARP_LOG_COMPONENT(MULTIPLEANALOGSENSORSCLIENTREMAPPER, "yarp.device.multipleanalogsensorsclientremapper")
}


void MultipleAnalogSensorsClientRemapper::closeAllMultipleAnalogSensorsClients()
{
    for(auto& device : m_multipleAnalogSensorsClientsDevices)
    {
        if( device != nullptr)
        {
            device->close();
            delete device;
            device = nullptr;
        }
    }

    m_multipleAnalogSensorsClientsDevices.resize(0);
}


bool MultipleAnalogSensorsClientRemapper::close()
{
    bool ret = true;

    bool ok = MultipleAnalogSensorsRemapper::detachAll();

    ret = ret && ok;

    ok = MultipleAnalogSensorsRemapper::close();

    ret = ret && ok;

    this->closeAllMultipleAnalogSensorsClients();

    return ret;
}

bool MultipleAnalogSensorsClientRemapper::open(Searchable& config)
{
    Property prop;
    prop.fromString(config.toString());

    std::string localPortPrefix;
    std::vector<std::string> multipleAnalogSensorsClientsPorts;

    // Check if the required parameters  are found
    if( prop.check("localPortPrefix") && prop.find("localPortPrefix").isString() )
    {
        localPortPrefix = prop.find("localPortPrefix").asString();
    }
    else
    {
        yCError(MULTIPLEANALOGSENSORSCLIENTREMAPPER) << "Parsing parameters: \"localPortPrefix\" should be a string.";
        return false;
    }

    Bottle *multipleAnalogSensorsClients=prop.find("multipleAnalogSensorsClients").asList();
    if(multipleAnalogSensorsClients==nullptr)
    {
        yCError(MULTIPLEANALOGSENSORSCLIENTREMAPPER) << "Parsing parameters: \"multipleAnalogSensorsClients\" should be followed by a list.";
        return false;
    }

    multipleAnalogSensorsClientsPorts.resize(multipleAnalogSensorsClients->size());
    for(size_t i=0; i < multipleAnalogSensorsClients->size(); i++)
    {
        multipleAnalogSensorsClientsPorts[i] = multipleAnalogSensorsClients->get(i).asString();
    }

    // Load the MULTIPLE_ANALOG_SENSORS_CLIENTS_OPTIONS, containing any additional option to pass to the multipleanalogsensorsclient
    Property multipleAnalogSensorsClientsOptions;

    Bottle & optionsGroupBot = prop.findGroup("MULTIPLE_ANALOG_SENSORS_CLIENTS_OPTIONS");
    if( !(optionsGroupBot.isNull()) )
    {
        multipleAnalogSensorsClientsOptions.fromString(optionsGroupBot.toString());
    }

    // Parameters loaded, open all the multipleanalogsensorsclient

    m_multipleAnalogSensorsClientsDevices.resize(multipleAnalogSensorsClientsPorts.size(), nullptr);

    PolyDriverList multipleAnalogSensorsClientsList;

    for(size_t client=0; client < multipleAnalogSensorsClientsPorts.size(); client++ )
    {
        std::string remote = multipleAnalogSensorsClientsPorts[client];
        std::string local = localPortPrefix+remote;

        Property options = multipleAnalogSensorsClientsOptions;
        options.put("device", "multipleanalogsensorsclient");
        options.put("local", local);
        options.put("remote", remote);

        m_multipleAnalogSensorsClientsDevices[client] = new PolyDriver();

        bool ok = m_multipleAnalogSensorsClientsDevices[client]->open(options);

        if( !ok || !(m_multipleAnalogSensorsClientsDevices[client]->isValid()) )
        {
            yCError(MULTIPLEANALOGSENSORSCLIENTREMAPPER) << "Opening multipleanalogsensorsclient with remote \"" << remote << "\", opening the device failed.";
            this->closeAllMultipleAnalogSensorsClients();
            return false;
        }

        // We use the remote name of the multipleanalogsensorsclient as the key for it, in absence of anything better
        multipleAnalogSensorsClientsList.push((m_multipleAnalogSensorsClientsDevices[client]),
                                              remote.c_str());
    }

    // Device opened, now we open the MultipleAnalogSensorsRemapper and then we call attachAll
    bool ok = MultipleAnalogSensorsRemapper::open(prop);

    if( !ok )
    {
        yCError(MULTIPLEANALOGSENSORSCLIENTREMAPPER) << "Opening the multipleanalogsensorsremapper device, opening the device failed.";
        MultipleAnalogSensorsRemapper::close();
        this->closeAllMultipleAnalogSensorsClients();
        return false;
    }

    // If open went ok, we now call attachAll
    ok = MultipleAnalogSensorsRemapper::attachAll(multipleAnalogSensorsClientsList);

    if( !ok )
    {
        yCError(MULTIPLEANALOGSENSORSCLIENTREMAPPER) << "Calling attachAll in the multipleanalogsensorsclient device, opening the device failed.";
        MultipleAnalogSensorsRemapper::close();
        this->closeAllMultipleAnalogSensorsClients();
        return false;
    }

    return true;
}

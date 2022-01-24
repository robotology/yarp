/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "AwaitSystemReady_nws_yarp.h"
#include <yarp/os/Network.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

YARP_LOG_COMPONENT(AWAITSYSTEMREADY_NWS_YARP, "yarp.devices.AwaitSystemReady_nws_yarp")

AwaitSystemReady_nws_yarp::AwaitSystemReady_nws_yarp()
{
}

bool AwaitSystemReady_nws_yarp::open(yarp::os::Searchable &config)
{
    if (config.check("PORT_LIST")) {
        // skips the first one since it is PORT_LIST
        yarp::os::Bottle port_list = config.findGroup("PORT_LIST").tail();
        for (int index = 0; index < port_list.size(); index++) {
            std::string current_port_property = port_list.get(index).toString();
            std::string token;
            int pos;
            char delimiter = ' ';
            pos = current_port_property.find(delimiter);
            token = current_port_property.substr(0, pos);
            std::string current_port_name = port_list.find(token).asString();
            yarp::os::NetworkBase::waitPort(current_port_name);
        }
        return true;
    }
    yCError(AWAITSYSTEMREADY_NWS_YARP) << "missing PORT_LIST group";
    return false;
}


bool AwaitSystemReady_nws_yarp::close()
{
    return true;
}

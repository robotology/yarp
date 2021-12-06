/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "SystemReady_nws_yarp.h"
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

YARP_LOG_COMPONENT(SYSTEMREADY_NWS_YARP, "yarp.devices.SystemReady_nws_yarp")

SystemReady_nws_yarp::SystemReady_nws_yarp()
{
}

bool SystemReady_nws_yarp::open(yarp::os::Searchable &config)
{
    if (config.check("PORT_LIST")) {
        yarp::os::Bottle port_list = config.findGroup("PORT_LIST");
        // skips the first one since it is PORT_LIST
        for (int index = 1; index < port_list.size(); index++) {
            std::string current_port_property = port_list.get(index).toString();
            std::string token;
            int pos;
            char delimiter = ' ';
            pos = current_port_property.find(delimiter);
            token = current_port_property.substr(0, pos);
            std::string current_port_name = port_list.find(token).asString();
            yCError(SYSTEMREADY_NWS_YARP) << port_list.get(index).asList()[0].toString();
            yCError(SYSTEMREADY_NWS_YARP) << current_port_property;
            yCError(SYSTEMREADY_NWS_YARP) << token;
            yCError(SYSTEMREADY_NWS_YARP) << port_list.find(token).asString();
            yarp::os::Port* current_port = new yarp::os::Port;
            current_port->open(current_port_name);
            port_pointers_list.push_back(current_port);
        }
        return true;
    }
    return false;
}


bool SystemReady_nws_yarp::close()
{
    for (auto elem: port_pointers_list){
        elem->close();
        delete elem;
    }
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/profiler/NetworkProfiler.h>
#include <yarp/os/Network.h>

int main(int argc, char *argv[])
{
    yarp::os::Network yarp;

    yarp::os::Property p;
    if (argc > 1) {
        p.fromCommand(argc, argv);
    }

    std::string from_ip = p.check("from_ip",yarp::os::Value(std::string("*")), "").asString();
    std::string to_ip = p.check("to_ip",yarp::os::Value(std::string("*")), "").asString();
    std::string from_portnumber = p.check("from_portnumber",yarp::os::Value(std::string("*")), "").asString();
    std::string to_portnumber = p.check("to_portnumber",yarp::os::Value(std::string("*")), "").asString();
    std::string from_portname = p.check("from_portname",yarp::os::Value(std::string("*")), "").asString();
    std::string to_portname = p.check("to_portname",yarp::os::Value(std::string("*")), "").asString();

    yarp::profiler::NetworkProfiler prof;
    yarp::profiler::NetworkProfiler::connections_set conns;
    yarp::profiler::NetworkProfiler::connections_set conns_filtered;

    prof.getConnectionsList(conns);

    if (from_ip != "*" || to_ip != "*")
    {
        prof.filterConnectionListByIp(conns, conns_filtered, from_ip,to_ip);
        conns= conns_filtered;
    }
    if (from_portnumber != "*" || to_portnumber != "*")
    {
        prof.filterConnectionListByPortNumber(conns, conns_filtered, from_portnumber, to_portnumber);
        conns = conns_filtered;
    }
    if (from_portname != "*" || to_portname != "*")
    {
        prof.filterConnectionListByName(conns, conns_filtered, from_portname, to_portname);
        conns = conns_filtered;
    }

    std::string sss = "Connections:\n";
    if (conns.size() !=0)
    for (auto it = conns.begin(); it!=conns.end(); it++)
    {
        sss += it->src.name + " (" + it->src.ip + ":" + it->src.port_number + ") -> " +
               it->dst.name + " (" + it->dst.ip + ":" + it->dst.port_number + ") with carrier: (" +
               it->carrier + ")\n";
    }
    else
    {
        sss +="Empty list";
    }

    yInfo() << sss;
    return 1;
}

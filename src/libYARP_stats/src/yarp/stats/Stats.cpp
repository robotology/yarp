/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/stats/Stats.h>

#include <cstdio>
#include <string>
#include <cstring>
#include <random>
#include <yarp/os/LogStream.h>
#include <yarp/profiler/NetworkProfiler.h>

using namespace yarp::stats;
using namespace yarp::os;

namespace
{
    YARP_LOG_COMPONENT(YARPSTATS, "yarp.StatsEngine")
}

void StatsEngine::disconnect ()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_local_port.close();
}

bool StatsEngine::connect (const std::list<std::string>& ports)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string conn_portname = m_local_portname + ":i";
    if (!m_local_port.open(conn_portname))
    {
        yCError(YARPSTATS) << "Unable to open local port" << conn_portname;
        return false;
    }
    m_local_port.setStrict();

    yarp::os::ContactStyle style;
    style.timeout=1.0;
    style.quiet=true;
    style.carrier = "fast_tcp";

    std::list<std::string>::const_iterator it;
    bool b_connected = true;
    for (it = ports.begin(); it != ports.end(); it++)
    {
        if (yarp::os::Network::exists(*it,style) == true)
        {
            b_connected &= yarp::os::Network::connect(*it, conn_portname, style);
        }
        else
        {
            //fprintf(stderr,"unable to connect to port %s\n",it->c_str());
        }
    }
    if (b_connected == false)
    {
        yCError(YARPSTATS) << "Unable to connect to some ports, check the network and the port names";
        return false;
    }
    return true;
}

bool StatsEngine::discover  (std::list<std::string>& ports, PortInfoMap& port_info_map)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    yarp::profiler::NetworkProfiler prof;
    yarp::profiler::NetworkProfiler::ports_name_set ports_list;
    prof.getPortsList(ports_list);

    for (const auto& portName : ports_list)
    {
        if (portName.name.find("/stats:o") != std::string::npos)
        {
            ports.push_back(portName.name);
        }
        port_info_map[portName.name] = {portName.ip, portName.port_number};
    }
    return true;
}

void StatsEngine::update(std::list<ConnectionStats>& stats)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    stats.clear();

    if (m_local_port.getInputCount()>0)
    {
        int bufferport_size = m_local_port.getPendingReads();

        while (bufferport_size>0)
        {
            ConnectionStats *b = m_local_port.read(); //this is blocking
            bufferport_size = m_local_port.getPendingReads();

            if (b==nullptr)
            {
                fprintf (stderr, "ERROR: something strange happened here, bufferport_size = %d!\n",bufferport_size);
                return;
            }

            if (b->isForward == false      && m_display_backward == false) { continue; }
            if (b->bytes_per_second == 0.0 && m_display_zero_bps == false) { continue; }

            stats.push_back(*b);
        }
    }
}

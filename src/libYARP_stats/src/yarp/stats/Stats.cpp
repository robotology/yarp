/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/stats/Stats.h>

#include <cstdio>
#include <string>
#include <cstring>
#include <random>

using namespace yarp::stats;
using namespace yarp::os;

void StatsEngine::disconnect ()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_local_port.close();
}

void StatsEngine::connect (const std::list<std::string>& ports)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string conn_portname = m_local_portname + ":i";
    m_local_port.open(conn_portname);
    m_local_port.setStrict();

    yarp::os::ContactStyle style;
    style.timeout=1.0;
    style.quiet=true;

    std::list<std::string>::const_iterator it;
    bool b_connected = true;
    for (it = ports.begin(); it != ports.end(); it++)
    {
        if (yarp::os::Network::exists(*it,style) == true)
        {
            b_connected &= yarp::os::Network::connect(*it, conn_portname);
        }
        else
        {
            //fprintf(stderr,"unable to connect to port %s\n",it->c_str());
        }
    }
}

void StatsEngine::discover  (std::list<std::string>& ports)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    RpcClient p;
    std::string discover_portname = m_local_portname + "/discover";
    p.open(discover_portname);
    std::string yarpservername = yarp::os::Network::getNameServerName();
    bool b_connected = yarp::os::Network::connect(discover_portname,yarpservername);
    Bottle cmd,response;
    cmd.addString("bot");
    cmd.addString("list");
    p.write(cmd,response);
    printf ("%s\n\n", response.toString().c_str());
    int size = response.size();
    for (int i=1; i<size; i++) //beware: skip i=0 is intentional!
    {
        Bottle* n1 = response.get(i).asList();
        if (n1 && n1->get(0).toString()=="port")
        {
            Bottle* n2 = n1->get(1).asList();
            if (n2 && n2->get(0).toString()=="name")
            {
                char* log_off = nullptr;
                char* yarprun_log_off = nullptr;
                log_off = std::strstr((char*)(n2->get(1).toString().c_str()), "/stats:o");
                if (log_off)
                {
                    std::string logport = n2->get(1).toString();
                    printf ("%s\n", logport.c_str());
                    ports.push_back(logport);
                }
            }
        }
    }
    p.close();
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
            if (b->bytes_per_second == 0.0 && m_display_zero_bps == true)  { continue; }

            stats.push_back(*b);
        }
    }
}

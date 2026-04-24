/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_PORTMONITOR_STATSMONITOR_H
#define YARP_PORTMONITOR_STATSMONITOR_H

#include <yarp/os/MonitorObject.h>

#include <yarp/os/Port.h>
#include <yarp/os/PeriodicThread.h>

#include <string>

class StatsThread :  public yarp::os::PeriodicThread
{
private:
    std::mutex m_mutex;
    double m_datasize_sum=0;
    double m_period = 1.0;
    int    m_datasize_count=0;
    std::string m_source;
    std::string m_destination;
    std::string m_stats_port_name = "/testport";
    yarp::os::Port m_stats_port;
    bool m_isForward = true;

public:
    StatsThread() = delete;;
    StatsThread(const StatsThread&) = delete;
    StatsThread(double period, std::string src, std::string dest, bool fw) : PeriodicThread(period)
    {
        m_period = period;
        m_source =src;
        m_destination=dest;
        m_stats_port_name = "/stats_monitor/" + m_source + m_destination + "/stats:o";
        m_isForward = fw;
    }
    virtual ~StatsThread() = default;

    void setData(size_t datasize);
    void run() override;
    bool threadInit() override;
};

 /**
  * @ingroup portmonitors_lists
 * \brief `stats_monitor`: When this port monitor is attached to the sender side of a connection, it
 * opens two additional output ports (one for forward data and one for backward data). These ports
 * provide statistics about the connection, such as bandwidth usage (in Bytes/s) and the number of
 * writes per second.
 * The names of the two ports are automatically generated according to the following pattern:
 * /stats_monitor/" + m_source + m_destination + "/stats:o" data from source to destination
 * /stats_monitor/" + m_destination + m_source + "/stats:o" data from destination back to source (for example in a rpc connection)
 */
class StatsMonitor :
        public yarp::os::MonitorObject
{
public:
    bool create(const yarp::os::Property& options) override;
    yarp::os::Things& update(yarp::os::Things& thing) override;
    yarp::os::Things& updateReply(yarp::os::Things& thing) override;

private:
    bool           m_isSender;
    std::string    m_source;
    std::string    m_destination;
    std::unique_ptr<StatsThread> m_stats_thread_forward;
    std::unique_ptr<StatsThread> m_stats_thread_backward;
};

#endif // YARP_PORTMONITOR_STATSMONITOR_H

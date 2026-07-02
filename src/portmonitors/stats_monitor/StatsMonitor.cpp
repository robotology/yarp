/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "StatsMonitor.h"

#include <map>
#include <sstream>

#include <yarp/os/Bottle.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Things.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>

#include <yarp/stats/ConnectionStats.h>

//yarp connect /mysender /myreceiver tcp+send.portmonitor+file.stats_monitor+type.dll

// Monitor: log component
namespace {
YARP_LOG_COMPONENT(STATSMONITOR,
                   "yarp.portmonitor.StatsMonitor",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
} // namespace

// Monitor: create
bool StatsMonitor::create(const yarp::os::Property& options)
{
    m_isSender = options.find("sender_side").asBool();
    m_source = options.find("source").asString();
    m_destination = options.find("destination").asString();

    if (!m_isSender) {
        yCError(STATSMONITOR, "Attaching on receiver side is not supported yet.");
        return false;
    }

    double period = 1.0;
    m_stats_thread_forward = std::make_unique<StatsThread>(period, m_source, m_destination, m_carrier, true);
    if (!m_stats_thread_forward->start())
    {
        return false;
    }
    m_stats_thread_backward = std::make_unique<StatsThread>(period, m_destination, m_source, m_carrier, false);
    if (!m_stats_thread_backward->start())
    {
        return false;
    }

    return true;
}

// Monitor: update
yarp::os::Things& StatsMonitor::update(yarp::os::Things& thing)
{
    //serialize the data, compute its size
    yarp::os::impl::BufferedConnectionWriter writer;
    thing.write(writer);
    size_t bytes = writer.dataSize();

    //update statistics
    m_stats_thread_forward->setData(bytes);

    //propagate data transmission
    return thing;
}

// Monitor: updateReply
yarp::os::Things& StatsMonitor::updateReply(yarp::os::Things& thing)
{
    //serialize the data, compute its size
    yarp::os::impl::BufferedConnectionWriter writer;
    thing.write(writer);
    size_t bytes = writer.dataSize();

    //update statistics
    m_stats_thread_backward->setData(bytes);

    //propagate data transmission
    return thing;
}

void StatsThread::setData(size_t datasize)
{
    std::lock_guard lock(m_mutex);
    m_datasize_count++;
    m_datasize_sum += datasize;

    //yDebug() << datasize << " " << m_datasize_sum << " " << m_datasize_count;
}

void StatsThread::run()
{
    double local_sum;
    int local_count;
    {
        std::lock_guard lock(m_mutex);
        local_sum = m_datasize_sum;
        local_count = m_datasize_count;
        m_datasize_count = 0;
        m_datasize_sum = 0;
    }

    yarp::stats::ConnectionStats msg;
    msg.timestamp = yarp::os::SystemClock::nowSystem();
    msg.source = m_source;
    msg.destination = m_destination;
    msg.carrier = m_carrier;
    msg.isForward = m_isForward;
    msg.bytes_per_second = local_sum/m_period;
    msg.frequency = local_count/m_period;
    m_stats_port.write(msg);

    //yDebug("Writing: %s", msg.toString().c_str());
}

bool StatsThread::threadInit()
{
   if (!m_stats_port.open(m_stats_port_name))
   {
       yCError(STATSMONITOR, "Could not open port %s.", m_stats_port_name.c_str());
       return false;
   }
   return true;
}

void StatsThread::threadRelease()
{
   m_stats_port.close();
}

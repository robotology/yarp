/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_STATS_STATS_H
#define YARP_STATS_STATS_H

#include <vector>
#include <list>
#include <mutex>

#include <cstring>
#include <yarp/stats/api.h>

#include <yarp/os/RpcServer.h>
#include <yarp/os/Property.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/ContactStyle.h>
#include <yarp/os/Network.h>
#include <yarp/os/RpcClient.h>

#include "yarp/stats/ConnectionStats.h"

namespace yarp::stats {

class YARP_stats_API StatsEngine
{
    std::mutex m_mutex;
    std::list<ConnectionStats> m_connections_stats;
    yarp::os::BufferedPort<ConnectionStats> m_local_port;
    std::string m_local_portname = "/yarpConnectionsStats";

public:
    bool m_display_backward = false;
    bool m_display_zero_bps = true;

    void disconnect();
    void connect (const std::list<std::string>& ports);
    void discover  (std::list<std::string>& ports);
    void update(std::list<ConnectionStats>& stats);
};

} // namespace yarp::stats


#endif // YARP_STATS_STATS_H

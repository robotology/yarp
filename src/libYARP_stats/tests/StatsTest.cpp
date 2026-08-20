/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/stats/Stats.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::stats;
using namespace yarp::os;

TEST_CASE("stats::Stats", "[yarp::stats]")
{
    NetworkBase::setLocalMode(true);

    SECTION("test engine")
    {
        StatsEngine engine;
        std::list<std::string> m_ports;
        yarp::stats::StatsEngine::PortInfoMap m_portsmap;

        bool b;
        b = engine.discover(m_ports, m_portsmap);
        b = engine.connect(m_ports);

        std::list<ConnectionStats> stats;
        engine.update(stats);

        ConnectionStats cs;

        engine.disconnect();
    }

    NetworkBase::setLocalMode(false);
}

/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RFModule.h>
#include <yarp/stats/Stats.h>

#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;
using namespace yarp::os;

namespace
{
    YARP_LOG_COMPONENT(YARPCONNINFO, "yarp.yarpconnectionsstats")
}

//---------------------------------------------------------------

class Stats_module : public yarp::os::RFModule
{
    void display_help ()
    {
        yCInfo(YARPCONNINFO) << "This is the tool yarpconnectionsstats, a tool that displays the frequency at which YARP ports broadcast data";
        yCInfo(YARPCONNINFO) << "";
        yCInfo(YARPCONNINFO) << "In order to work properly, the connection we want to inspect must be monitored by a `stats_monitor` portmonitor.";
        yCInfo(YARPCONNINFO) << "e.g. yarp connect /sender /recv tcp+send.portmonitor+file.stats_monitor+type.dll";
        yCInfo(YARPCONNINFO) << "";
        yCInfo(YARPCONNINFO) << "Additional options (default value: false):";
        yCInfo(YARPCONNINFO) << "--display_backward (default: false) if enabled";
        yCInfo(YARPCONNINFO) << "--display_zero_bps (default: true) if enabled, displays also if the connection is not transmitting anything";
        yCInfo(YARPCONNINFO) << "--display_by_source_name order results by source port name";
        yCInfo(YARPCONNINFO) << "--display_by_dest_name order results by destination port name";
        yCInfo(YARPCONNINFO) << "--display_by_bps order results by data size";
        yCInfo(YARPCONNINFO) << "--display_screen_frequency (default value: 1) refresh frequency";
    }

    enum class SortMode
    {
        Source,
        Destination,
        BytesPerSecondDesc
    };

    bool display_backward = false;
    bool display_zero_bps = true;
    bool display_by_source_name = false;
    bool display_by_dest_name = false;
    bool display_by_bps = false;
    double display_screen_frequency = 1.0;

    yarp::stats::StatsEngine engine;
    std::list<std::string> ports;

    bool configure(yarp::os::ResourceFinder &rf) override
    {
        if (rf.check("help"))                      { display_help(); return false; }
        if (rf.check("display_backward"))          { display_backward = rf.find("display_backward").asBool(); }
        if (rf.check("display_zero_bps"))          { display_zero_bps = rf.find("display_zero_bps").asBool(); }
        if (rf.check("display_by_source_name"))    { display_by_source_name = rf.find("display_by_source_name").asBool(); }
        if (rf.check("display_by_dest_name"))      { display_by_dest_name = rf.find("display_by_dest_name").asBool(); }
        if (rf.check("display_by_bps"))            { display_by_bps = rf.find("display_by_bps").asBool(); }
        if (rf.check("display_screen_frequency"))  { display_screen_frequency = rf.find("display_screen_frequency").asFloat64(); }

        engine.m_display_backward = display_backward;
        engine.m_display_zero_bps = display_zero_bps;

        engine.discover(ports);
        engine.connect(ports);

        return true;
    }

    bool close() override
    {
        engine.disconnect();
        return true;
    }

    double getPeriod() override
    {
        return display_screen_frequency;
    }

    bool updateModule() override
    {
        std::list<yarp::stats::ConnectionStats> stats;
        engine.update(stats);

        if      (display_by_source_name) {sortConnections(stats, SortMode::Source);}
        else if (display_by_dest_name)   {sortConnections(stats, SortMode::Destination);}
        else if (display_by_bps)         {sortConnections(stats, SortMode::BytesPerSecondDesc);}

        //Print the stats on screen
        std::cout << "\033[2J\033[H"; //clear the screen
        for (auto it=stats.begin(); it!=stats.end(); it++)
        {
            std::ostringstream line;

            line << std::left
                 << std::setw(25) << it->source
                 << " -> "
                 << std::setw(25) << it->destination
                 << " | "
                 << std::right
                 << std::setw(12) << it->bytes_per_second << " B/s"
                 << " | "
                 << std::setw(8) << it->frequency << " pkt/s"
                 << " | "
              //   << (it->isForward ? "forward" : "backward")
              //   << " | "
                 << std::setw(8) << it->carrier
                 << " | "
                 << std::setw(8) << std::fixed << std::setprecision(3) << it->timestamp;

            yCInfo(YARPCONNINFO) << line.str();
        }
        return true;
    }

    void sortConnections(std::list<yarp::stats::ConnectionStats>& connections, SortMode mode)
    {
        switch (mode)
        {
            case SortMode::Source:
                connections.sort(
                    [](const auto& a, const auto& b)
                    {
                        return a.source < b.source;
                    });
                break;

            case SortMode::Destination:
                connections.sort(
                    [](const auto& a, const auto& b)
                    {
                        return a.destination < b.destination;
                    });
                break;

            case SortMode::BytesPerSecondDesc:
                connections.sort(
                    [](const auto& a, const auto& b)
                    {
                        return a.bytes_per_second > b.bytes_per_second;
                    });
                break;
        }
    }
};

//---------------------------------------------------------------

int main(int argc, char *argv[])
{
    yarp::os::Network yarp;
    if (!yarp.checkNetwork())
    {
        fprintf(stderr,"ERROR: check YARP network.\n");
        return -1;
    }

    yarp::os::ResourceFinder rf;
    //rf.setDefaultConfigFile("yarpconnectionsstats.ini"); //overridden by --from parameter
    //rf.setDefaultContext("yarpconnectionsstats");        //overridden by --context parameter
    rf.configure(argc,argv);

    Stats_module statsm;
    return statsm.runModule(rf);
}

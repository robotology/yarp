/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>
#include <yarp/profiler/NetworkProfilerBasic.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

#include <yarp/os/Thread.h>
#include <yarp/serversql/Server.h>

using namespace yarp::profiler;

// A minimal thread to run the YARP server in-process
class YarpServerThread : public yarp::os::Thread
{
    yarp::serversql::Server ys;
    int    argc{0};
    char** argv{nullptr};

    void run() override
    {
        ys.run(argc, argv);
    }

    void onStop() override
    {
        ys.stop();
    }

public:
    void configure(int inArgc, char** inArgv)
    {
        argc = inArgc;
        argv = inArgv;
    }
};

TEST_CASE("profiler::NetworkProfilerBasicTest", "[yarp::profiler]")
{
    yarp::os::Network net;

    // Start a real name server (opens /root) in-process, storing data only in memory
    const char* argv[] = {"yarpserver", "--portdb", ":memory:", "--subdb", ":memory:", "--silent", "--write"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    YarpServerThread yServer;
    yServer.configure(argc, const_cast<char**>(argv));
    yServer.start();

    // Wait for the server to be up and /root reachable
    double startTime = yarp::os::Time::now();
    while (!net.checkNetwork() && (yarp::os::Time::now() - startTime) < 5.0)
    {
        yarp::os::Time::delay(0.1);
    }
    REQUIRE(net.checkNetwork());

    //This delay is needed to let the yarpserver to be fully up and running,
    //it seems that even if checkNetwork() is ok, you cannot open a port
    //immediately after, so we need to wait a bit more
    yarp::os::Time::delay(0.2);

    //Here the test code starts
    SECTION("Test several static methods")
    {
        bool bport;
        yarp::os::Port port1;
        bport = port1.open("/testPort/p1");
        REQUIRE(bport);
        yarp::os::Time::delay(0.2);

        yarp::os::Port port2;
        bport = port2.open("/testPort/p2");
        REQUIRE(bport);
        yarp::os::Time::delay(0.2);

        bport = yarp::os::Network::connect("/testPort/p1", "/testPort/p2", "tcp", false);
        REQUIRE(bport);
        yarp::os::Time::delay(0.2);
        REQUIRE(yarp::os::Network::isConnected("/testPort/p1", "/testPort/p2"));

        {
            bool b;
            NetworkProfilerBasic::PortDetails info;
            b =  NetworkProfilerBasic::getPortDetails("/testPort/p1", info);
            CHECK(b);
            CHECK(info.info.name == "/testPort/p1");
            CHECK(info.outputs.size()==1);
            CHECK(info.inputs.size()==0);
            b =  NetworkProfilerBasic::getPortDetails("/testPort/p2", info);
            CHECK(b);
            CHECK(info.info.name == "/testPort/p2");
            CHECK(info.inputs.size()==1);
            CHECK(info.outputs.size()==0);
            CHECK(info.owner_process.pid != 0);
            //CHECK(info.owner_process.arguments == "");
            CHECK(info.owner_process.process_name != "");
            CHECK(info.owner_process.process_fullname != "");
        }

        {
            bool b;
            NetworkProfilerBasic::ports_name_set ports_nset;
            b =  NetworkProfilerBasic::getPortsList(ports_nset, false);
            CHECK(b);
            REQUIRE(ports_nset.size() == 2); //p1, p2

            b =  NetworkProfilerBasic::getPortsList(ports_nset, true);
            CHECK(b);
            REQUIRE(ports_nset.size() == 3); //p1, p2, root

            std::string portName = "/testPort/p1";
            NetworkProfilerBasic::PortInfo pinfo;
            b = NetworkProfilerBasic::getPortInfo(portName, ports_nset, pinfo);
            CHECK(b);
            CHECK(pinfo.name == "/testPort/p1");
            CHECK(pinfo.port_number != "");
            CHECK(pinfo.ip != "");
        }

        {
            bool b;
            NetworkProfilerBasic::ports_detail_set ports;
            b = NetworkProfilerBasic::getPortsDetailedList(ports, false);
            CHECK(b);
            REQUIRE(ports.size() == 2);  //p1, p2

            b = NetworkProfilerBasic::getPortsDetailedList(ports, true);
            CHECK(b);
            REQUIRE(ports.size() == 3);  //p1, p2, root

            NetworkProfilerBasic::machines_list machines;
            b =  NetworkProfilerBasic::getMachinesList(ports, machines);
            CHECK(b);
            REQUIRE(machines.size()>0);
            CHECK(machines[0] != "");

            NetworkProfilerBasic::processes_list processes;
            b =  NetworkProfilerBasic::getProcessesList(ports, processes);
            CHECK(b);
            REQUIRE(processes.size()>0);
            CHECK(processes[0] != "");
        }

        {
            bool b;
            NetworkProfilerBasic::connections_set connections;
            b =  NetworkProfilerBasic::getConnectionsList(connections);
            CHECK(b);
            REQUIRE(connections.size() == 1);
            CHECK(connections[0].src.name == "/testPort/p1");
            CHECK(connections[0].dst.name == "/testPort/p2");
            CHECK(connections[0].carrier == "tcp");
        }

        port1.close();
        port2.close();
    }

    //Close the yarp server
    yServer.stop();
}

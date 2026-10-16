/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/PortCoreClient.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Value.h>
#include <yarp/os/YarpPluginSelector.h>

#include <algorithm>
#include <string>
#include <vector>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;

TEST_CASE("os::PortCoreClientTest", "[yarp::os]")
{
    NetworkBase::setLocalMode(true);

    SECTION("getAdminProtocolVersion")
    {
        BufferedPort<Bottle> p1;
        REQUIRE(p1.open("/p1"));

        PortCoreClient client("/p1");
        const return_getAdminProtocolVersion ver = client.getAdminProtocolVersion();
        REQUIRE(ver.ret);
        CHECK(ver.version_major == 1);
        CHECK(ver.version_minor == 2);
        CHECK(ver.version_patch == 4);

        p1.close();
    }

    SECTION("detachPortmonitorIn/detachPortmonitorOut on an existing port")
    {
        BufferedPort<Bottle> p1;
        REQUIRE(p1.open("/p1"));

        PortCoreClient client("/p1");
        CHECK(client.detachPortmonitorIn());
        CHECK(client.detachPortmonitorOut());

        p1.close();
    }

    SECTION("attachPortmonitorIn/attachPortmonitorOut with a portmonitor which cannot be configured")
    {
        BufferedPort<Bottle> p1;
        REQUIRE(p1.open("/p1"));

        PortCoreClient client("/p1");
        CHECK_FALSE(client.attachPortmonitorIn("(type nonexistent_type) (file nonexistent_file)"));
        CHECK_FALSE(client.attachPortmonitorOut("(type nonexistent_type) (file nonexistent_file)"));

        // the port is still able to process the following commands
        CHECK(client.detachPortmonitorIn());
        CHECK(client.detachPortmonitorOut());

        p1.close();
    }

    SECTION("attach/detach of the test_numbers1 portmonitor (adds 1 to each value)")
    {
        YARP_REQUIRE_PLUGIN("test_numbers1", "portmonitor");

        const std::string pm_options = "(type dll) (file test_numbers1)";

        Port sender;
        BufferedPort<Value> receiver;
        receiver.setStrict();
        REQUIRE(sender.open("/send"));
        REQUIRE(receiver.open("/recv"));
        REQUIRE(NetworkBase::connect("/send", "/recv"));

        auto sendAndReceive = [&](int sent) {
            sender.write(Value(sent));
            Value* received = receiver.read();
            REQUIRE(received != nullptr);
            return received->asInt64();
        };

        CHECK(sendAndReceive(1) == 1);

        // receiver side
        PortCoreClient recvClient("/recv");
        REQUIRE(recvClient.attachPortmonitorIn(pm_options));
        CHECK(sendAndReceive(1) == 2);
        REQUIRE(recvClient.detachPortmonitorIn());
        CHECK(sendAndReceive(1) == 1);

        // sender side
        PortCoreClient sendClient("/send");
        REQUIRE(sendClient.attachPortmonitorOut(pm_options));
        CHECK(sendAndReceive(1) == 2);

        // both sides
        REQUIRE(recvClient.attachPortmonitorIn(pm_options));
        CHECK(sendAndReceive(1) == 3);

        REQUIRE(sendClient.detachPortmonitorOut());
        REQUIRE(recvClient.detachPortmonitorIn());
        CHECK(sendAndReceive(1) == 1);

        receiver.close();
        sender.close();
    }

    SECTION("listConnectionsIn/Out and getConnectionInfoIn/Out")
    {
        Port sender;
        BufferedPort<Bottle> receiver;
        REQUIRE(sender.open("/send"));
        REQUIRE(receiver.open("/recv"));
        REQUIRE(NetworkBase::connect("/send", "/recv"));

        PortCoreClient sendClient("/send");
        PortCoreClient recvClient("/recv");

        auto contains = [](const std::vector<std::string>& v, const std::string& s) {
            return std::find(v.begin(), v.end(), s) != v.end();
        };

        // list
        const return_listConnections outputs = sendClient.listConnectionsOut();
        REQUIRE(outputs.ret);
        CHECK(outputs.connections == std::vector<std::string>{"/recv"});

        const return_listConnections inputs = recvClient.listConnectionsIn();
        REQUIRE(inputs.ret);
        CHECK(contains(inputs.connections, "/send"));

        CHECK(recvClient.listConnectionsOut().ret);
        CHECK(recvClient.listConnectionsOut().connections.empty());
        CHECK_FALSE(contains(sendClient.listConnectionsIn().connections, "/recv"));

        // details, from both sides of the connection
        for (const return_getConnectionInfo& info : {sendClient.getConnectionInfoOut("/recv"),
                                                     recvClient.getConnectionInfoIn("/send")}) {
            REQUIRE(info.ret);
            CHECK(info.found);
            CHECK(info.from_port == "/send");
            CHECK(info.to_port == "/recv");
            CHECK(info.carrier == "tcp");
            CHECK_FALSE(info.connectionless);
            CHECK(info.push);
        }

        // details of a connection which does not exist
        const return_getConnectionInfo missing = sendClient.getConnectionInfoOut("/nonexistent");
        CHECK(missing.ret);
        CHECK_FALSE(missing.found);
        CHECK_FALSE(recvClient.getConnectionInfoOut("/send").found);

        receiver.close();
        sender.close();
    }

    SECTION("addConnection/delConnection")
    {
        Port sender;
        BufferedPort<Bottle> receiver;
        REQUIRE(sender.open("/send"));
        REQUIRE(receiver.open("/recv"));

        PortCoreClient sendClient("/send");

        // add, without and with a carrier
        return_addConnection added = sendClient.addConnection("/recv");
        REQUIRE(added.ret);
        CHECK(added.success);
        CHECK_FALSE(added.message.empty());
        CHECK(NetworkBase::isConnected("/send", "/recv"));
        CHECK(sendClient.getConnectionInfoOut("/recv").carrier == "tcp");

        return_delConnection removed = sendClient.delConnection("/recv");
        REQUIRE(removed.ret);
        CHECK(removed.success);
        CHECK_FALSE(removed.message.empty());
        CHECK_FALSE(NetworkBase::isConnected("/send", "/recv"));

        added = sendClient.addConnection("/recv", "fast_tcp");
        REQUIRE(added.ret);
        CHECK(added.success);
        CHECK(sendClient.getConnectionInfoOut("/recv").carrier == "fast_tcp");

        // del removes also the input connections
        PortCoreClient recvClient("/recv");
        removed = recvClient.delConnection("/send");
        REQUIRE(removed.ret);
        CHECK(removed.success);
        CHECK_FALSE(NetworkBase::isConnected("/send", "/recv"));

        // nothing to remove / nothing to connect to
        removed = sendClient.delConnection("/nonexistent");
        REQUIRE(removed.ret);
        CHECK_FALSE(removed.success);

        added = sendClient.addConnection("/nonexistent");
        REQUIRE(added.ret);
        CHECK_FALSE(added.success);

        receiver.close();
        sender.close();
    }

    SECTION("addConnection fails when the port does not allow the connection")
    {
        BufferedPort<Bottle> recv1;
        BufferedPort<Bottle> recv2;
        REQUIRE(recv1.open("/recv1"));
        REQUIRE(recv2.open("/recv2"));

        // a port which does not allow outputs
        Port readOnly;
        readOnly.setReadOnly();
        REQUIRE(readOnly.open("/readonly"));
        PortCoreClient readOnlyClient("/readonly");
        return_addConnection added = readOnlyClient.addConnection("/recv1");
        REQUIRE(added.ret);
        CHECK_FALSE(added.success);
        CHECK_FALSE(added.message.empty());
        CHECK_FALSE(NetworkBase::isConnected("/readonly", "/recv1"));

        // an rpc client allows only one output
        Port rpcClient;
        rpcClient.setRpcClient();
        REQUIRE(rpcClient.open("/rpcclient"));
        PortCoreClient rpcClientClient("/rpcclient");
        added = rpcClientClient.addConnection("/recv1");
        REQUIRE(added.ret);
        CHECK(added.success);
        added = rpcClientClient.addConnection("/recv2");
        REQUIRE(added.ret);
        CHECK_FALSE(added.success);
        CHECK_FALSE(added.message.empty());
        CHECK(NetworkBase::isConnected("/rpcclient", "/recv1"));
        CHECK_FALSE(NetworkBase::isConnected("/rpcclient", "/recv2"));

        rpcClient.close();
        readOnly.close();
        recv2.close();
        recv1.close();
    }

    SECTION("delConnection removes both the input and the output connections")
    {
        BufferedPort<Bottle> a;
        BufferedPort<Bottle> b;
        REQUIRE(a.open("/a"));
        REQUIRE(b.open("/b"));
        REQUIRE(NetworkBase::connect("/a", "/b"));
        REQUIRE(NetworkBase::connect("/b", "/a"));

        PortCoreClient aClient("/a");
        const return_delConnection removed = aClient.delConnection("/b");
        REQUIRE(removed.ret);
        CHECK(removed.success);
        // the connections are removed synchronously on the side of /a
        // (/b notices the removal asynchronously)
        CHECK(aClient.listConnectionsOut().connections.empty());
        const std::vector<std::string> inputs = aClient.listConnectionsIn().connections;
        CHECK(std::find(inputs.begin(), inputs.end(), "/b") == inputs.end());

        b.close();
        a.close();
    }

    SECTION("commands sent through a port already connected in admin mode")
    {
        BufferedPort<Bottle> p1;
        Port ping;
        REQUIRE(p1.open("/p1"));
        REQUIRE(ping.open("/ping"));
        REQUIRE(NetworkBase::connect("/ping", "/p1"));
        ping.setAdminMode();

        PortCoreClient client(ping);
        const return_listConnections inputs = client.listConnectionsIn();
        REQUIRE(inputs.ret);
        CHECK(inputs.connections == std::vector<std::string>{"/ping"});
        CHECK(client.detachPortmonitorIn());

        ping.close();
        p1.close();
    }

    SECTION("all the methods fail on a non existing port")
    {
        PortCoreClient client("/nonexistent_port", 0.5);
        CHECK_FALSE(client.attachPortmonitorIn("(file nonexistent_file)"));
        CHECK_FALSE(client.attachPortmonitorOut("(file nonexistent_file)"));
        CHECK_FALSE(client.detachPortmonitorIn());
        CHECK_FALSE(client.detachPortmonitorOut());
        CHECK_FALSE(client.listConnectionsIn().ret);
        CHECK_FALSE(client.listConnectionsOut().ret);
        CHECK_FALSE(client.getConnectionInfoIn("/p1").ret);
        CHECK_FALSE(client.getConnectionInfoOut("/p1").ret);
        CHECK_FALSE(client.getAdminProtocolVersion().ret);
        CHECK_FALSE(client.addConnection("/p1").ret);
        CHECK_FALSE(client.delConnection("/p1").ret);
    }

    NetworkBase::setLocalMode(false);
}

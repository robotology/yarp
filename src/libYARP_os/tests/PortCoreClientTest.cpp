/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/PortCoreClient.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Property.h>
#include <yarp/os/QosStyle.h>
#include <yarp/os/SystemInfo.h>
#include <yarp/os/Value.h>
#include <yarp/os/YarpPluginSelector.h>

#include <algorithm>
#include <string>
#include <vector>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;

// Tests of the administrative commands of the YARP ports (the thrift service
// PortCoreMsgs, see idl/PortCoreMsgs.thrift), sent through the client
// yarp::os::impl::PortCoreClient and handled by PortCore::adminBlock().
//
// Every method of the service is tested:
// - on its normal behaviour, checking the effect on the port or on the data
//   which travels on the connection (not just the reply);
// - on its failure cases (no such connection, wrong direction, port which does
//   not allow the operation, missing portmonitor);
// - against a port which does not exist (the field "ret" of the reply is false).
//
// The "In" methods refer to a connection coming from the given port to the
// queried port, the "Out" methods to a connection going from the queried port
// to the given port.
//
// The sections using the test_numbers1 portmonitor (which adds a value, by
// default 1, to each number it receives) are skipped if the plugin is not
// available.

TEST_CASE("os::PortCoreClientTest", "[yarp::os]")
{
    NetworkBase::setLocalMode(true);

    // Version of the administrative protocol (it is distinct from the YARP version).
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

    // Information about the port, its main thread, its process and its platform:
    // the values must match the ones of the current process, and the flags of
    // the port must reflect its configuration.
    SECTION("getPortInfo")
    {
        BufferedPort<Bottle> p1;
        Port readOnly;
        readOnly.setReadOnly();
        REQUIRE(p1.open("/p1"));
        REQUIRE(readOnly.open("/readonly"));

        PortCoreClient client("/p1");
        const return_getPortInfo info = client.getPortInfo();
        REQUIRE(info.ret);
        CHECK(info.process.pid == SystemInfo::getProcessInfo().pid);
        CHECK(info.process.name == SystemInfo::getProcessInfo().name);
        CHECK(info.platform.name == SystemInfo::getPlatformInfo().name);
        CHECK(info.port.is_input);
        CHECK(info.port.is_output);
        CHECK_FALSE(info.port.is_rpc);
        CHECK_FALSE(info.port.hostname.empty());

        // the flags reflect the configuration of the port
        const return_getPortInfo readOnlyInfo = PortCoreClient("/readonly").getPortInfo();
        REQUIRE(readOnlyInfo.ret);
        CHECK(readOnlyInfo.port.is_input);
        CHECK_FALSE(readOnlyInfo.port.is_output);

        readOnly.close();
        p1.close();
    }

    // Detaching when no portmonitor is attached is not an error.
    SECTION("detachPortmonitorIn/detachPortmonitorOut on an existing port")
    {
        BufferedPort<Bottle> p1;
        REQUIRE(p1.open("/p1"));

        PortCoreClient client("/p1");
        CHECK(client.detachPortmonitorIn());
        CHECK(client.detachPortmonitorOut());

        p1.close();
    }

    // A portmonitor which cannot be created is reported as a failure, and it
    // does not leave the port in an inconsistent state.
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

    // A portmonitor attached to the input of the receiver, to the output of the
    // sender, or to both, modifies the data which travels on the connection; after
    // the detach the data is not modified anymore.
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

    // The parameters of a portmonitor cannot be set nor read if no portmonitor is
    // attached: success is false and the message explains why.
    SECTION("setPortmonitorParamsIn/Out and getPortmonitorParamsIn/Out without a portmonitor")
    {
        BufferedPort<Bottle> p1;
        REQUIRE(p1.open("/p1"));

        PortCoreClient client("/p1");
        for (const return_setPortmonitorParams& result : {client.setPortmonitorParamsIn("(increment 5)"),
                                                          client.setPortmonitorParamsOut("(increment 5)")}) {
            REQUIRE(result.ret);
            CHECK_FALSE(result.success);
            CHECK_FALSE(result.message.empty());
        }
        for (const return_getPortmonitorParams& result : {client.getPortmonitorParamsIn(),
                                                          client.getPortmonitorParamsOut()}) {
            REQUIRE(result.ret);
            CHECK_FALSE(result.success);
            CHECK(result.params.empty());
            CHECK_FALSE(result.message.empty());
        }

        p1.close();
    }

    // The parameters are passed to the portmonitor attached to the port (its
    // setparam/getparam methods): changing "increment" changes the data received,
    // and the new value can be read back.
    SECTION("setPortmonitorParamsIn/Out and getPortmonitorParamsIn/Out with the test_numbers1 portmonitor")
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

        auto getIncrement = [](const std::string& params) {
            Property property;
            property.fromString(params);
            return property.find("increment").asInt32();
        };

        // receiver side
        PortCoreClient recvClient("/recv");
        REQUIRE(recvClient.attachPortmonitorIn(pm_options));
        CHECK(sendAndReceive(1) == 2);

        return_getPortmonitorParams params = recvClient.getPortmonitorParamsIn();
        REQUIRE(params.ret);
        REQUIRE(params.success);
        CHECK(getIncrement(params.params) == 1);

        const return_setPortmonitorParams set = recvClient.setPortmonitorParamsIn("(increment 5)");
        REQUIRE(set.ret);
        CHECK(set.success);
        CHECK(sendAndReceive(1) == 6);
        params = recvClient.getPortmonitorParamsIn();
        REQUIRE(params.success);
        CHECK(getIncrement(params.params) == 5);

        // the output has no portmonitor attached
        CHECK_FALSE(recvClient.getPortmonitorParamsOut().success);
        REQUIRE(recvClient.detachPortmonitorIn());

        // sender side
        PortCoreClient sendClient("/send");
        REQUIRE(sendClient.attachPortmonitorOut(pm_options));
        CHECK(sendClient.setPortmonitorParamsOut("(increment 10)").success);
        CHECK(sendAndReceive(1) == 11);
        params = sendClient.getPortmonitorParamsOut();
        REQUIRE(params.success);
        CHECK(getIncrement(params.params) == 10);
        REQUIRE(sendClient.detachPortmonitorOut());

        // after the detach the parameters are not available anymore
        CHECK_FALSE(sendClient.getPortmonitorParamsOut().success);
        CHECK(sendAndReceive(1) == 1);

        receiver.close();
        sender.close();
    }

    // The carrier parameters of a connection can be set and read from both its
    // ends; a connection which does not exist, or which goes in the other
    // direction, is reported as a failure.
    SECTION("setConnectionParamsIn/Out and getConnectionParamsIn/Out")
    {
        Port sender;
        BufferedPort<Bottle> receiver;
        REQUIRE(sender.open("/send"));
        REQUIRE(receiver.open("/recv"));
        REQUIRE(NetworkBase::connect("/send", "/recv"));

        PortCoreClient sendClient("/send");
        PortCoreClient recvClient("/recv");

        // existing connections, from both sides
        return_setConnectionParams set = sendClient.setConnectionParamsOut("/recv", "(key 1)");
        REQUIRE(set.ret);
        CHECK(set.success);
        set = recvClient.setConnectionParamsIn("/send", "(key 1)");
        REQUIRE(set.ret);
        CHECK(set.success);

        return_getConnectionParams params = sendClient.getConnectionParamsOut("/recv");
        REQUIRE(params.ret);
        CHECK(params.success);
        params = recvClient.getConnectionParamsIn("/send");
        REQUIRE(params.ret);
        CHECK(params.success);

        // connections which do not exist (wrong port, or wrong direction)
        for (const return_setConnectionParams& result : {sendClient.setConnectionParamsOut("/nonexistent", "(key 1)"),
                                                         sendClient.setConnectionParamsIn("/recv", "(key 1)"),
                                                         recvClient.setConnectionParamsOut("/send", "(key 1)")}) {
            REQUIRE(result.ret);
            CHECK_FALSE(result.success);
            CHECK_FALSE(result.message.empty());
        }
        for (const return_getConnectionParams& result : {sendClient.getConnectionParamsOut("/nonexistent"),
                                                         sendClient.getConnectionParamsIn("/recv"),
                                                         recvClient.getConnectionParamsOut("/send")}) {
            REQUIRE(result.ret);
            CHECK_FALSE(result.success);
            CHECK(result.params.empty());
            CHECK_FALSE(result.message.empty());
        }

        receiver.close();
        sender.close();
    }

    // On a connection using the portmonitor carrier (recv.portmonitor on the
    // receiver side, send.portmonitor on the sender side) the carrier parameters
    // are forwarded to the portmonitor of the connection, and change the data
    // received.
    SECTION("setConnectionParamsIn/Out and getConnectionParamsIn/Out on connections with the test_numbers1 portmonitor carrier")
    {
        YARP_REQUIRE_PLUGIN("test_numbers1", "portmonitor");

        Port sender;
        BufferedPort<Value> receiver;
        receiver.setStrict();
        REQUIRE(sender.open("/send"));
        REQUIRE(receiver.open("/recv"));

        auto sendAndReceive = [&](int sent) {
            sender.write(Value(sent));
            Value* received = receiver.read();
            REQUIRE(received != nullptr);
            return received->asInt64();
        };

        auto getIncrement = [](const return_getConnectionParams& params) {
            Property property;
            property.fromString(params.params);
            return property.find("increment").asInt32();
        };

        // portmonitor on the receiver side of the connection
        REQUIRE(NetworkBase::connect("/send", "/recv", "fast_tcp+recv.portmonitor+type.dll+file.test_numbers1"));
        CHECK(sendAndReceive(1) == 2);

        PortCoreClient recvClient("/recv");
        const return_setConnectionParams setIn = recvClient.setConnectionParamsIn("/send", "(increment 3)");
        REQUIRE(setIn.ret);
        CHECK(setIn.success);
        CHECK(sendAndReceive(1) == 4);

        const return_getConnectionParams paramsIn = recvClient.getConnectionParamsIn("/send");
        REQUIRE(paramsIn.ret);
        REQUIRE(paramsIn.success);
        CHECK(getIncrement(paramsIn) == 3);

        REQUIRE(NetworkBase::disconnect("/send", "/recv"));

        // portmonitor on the sender side of the connection
        REQUIRE(NetworkBase::connect("/send", "/recv", "fast_tcp+send.portmonitor+type.dll+file.test_numbers1"));
        CHECK(sendAndReceive(1) == 2);

        PortCoreClient sendClient("/send");
        const return_setConnectionParams setOut = sendClient.setConnectionParamsOut("/recv", "(increment 7)");
        REQUIRE(setOut.ret);
        CHECK(setOut.success);
        CHECK(sendAndReceive(1) == 8);

        const return_getConnectionParams paramsOut = sendClient.getConnectionParamsOut("/recv");
        REQUIRE(paramsOut.ret);
        REQUIRE(paramsOut.success);
        CHECK(getIncrement(paramsOut) == 7);

        receiver.close();
        sender.close();
    }

    // The connections are listed, and described, from both their ends.
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

    // The details of the connection reflect the properties of its carrier.
    SECTION("getConnectionInfoIn/Out on a connectionless carrier")
    {
        Port sender;
        BufferedPort<Bottle> receiver;
        REQUIRE(sender.open("/send"));
        REQUIRE(receiver.open("/recv"));
        REQUIRE(NetworkBase::connect("/send", "/recv", "udp"));

        const return_getConnectionInfo info = PortCoreClient("/send").getConnectionInfoOut("/recv");
        REQUIRE(info.ret);
        REQUIRE(info.found);
        CHECK(info.carrier == "udp");
        CHECK(info.connectionless);
        CHECK(info.push);

        receiver.close();
        sender.close();
    }

    // The qos (thread scheduling and packets type of service) of a connection
    // can be read and set from both its ends; a connection which does not exist,
    // or which goes in the other direction, is reported as a failure.
    SECTION("getConnectionQosIn/Out and setConnectionQosIn/Out")
    {
        Port sender;
        BufferedPort<Bottle> receiver;
        REQUIRE(sender.open("/send"));
        REQUIRE(receiver.open("/recv"));
        REQUIRE(NetworkBase::connect("/send", "/recv"));

        PortCoreClient sendClient("/send");
        PortCoreClient recvClient("/recv");

        // existing connections, from both sides
        for (const return_getConnectionQos& qos : {sendClient.getConnectionQosOut("/recv"),
                                                   recvClient.getConnectionQosIn("/send")}) {
            REQUIRE(qos.ret);
            CHECK(qos.found);
        }
        for (const return_setConnectionQos& result : {sendClient.setConnectionQosOut("/recv", -1, -1, 0),
                                                      recvClient.setConnectionQosIn("/send", -1, -1, 0)}) {
            REQUIRE(result.ret);
            CHECK(result.success);
        }

        // connections which do not exist (wrong port, or wrong direction)
        for (const return_getConnectionQos& qos : {sendClient.getConnectionQosOut("/nonexistent"),
                                                   sendClient.getConnectionQosIn("/recv"),
                                                   recvClient.getConnectionQosOut("/send")}) {
            REQUIRE(qos.ret);
            CHECK_FALSE(qos.found);
            CHECK(qos.qos_tos == -1);
        }
        for (const return_setConnectionQos& result : {sendClient.setConnectionQosOut("/nonexistent", -1, -1, 0),
                                                      sendClient.setConnectionQosIn("/recv", -1, -1, 0),
                                                      recvClient.setConnectionQosOut("/send", -1, -1, 0)}) {
            REQUIRE(result.ret);
            CHECK_FALSE(result.success);
            CHECK_FALSE(result.message.empty());
        }

        receiver.close();
        sender.close();
    }

    // When two ports are connected in both directions, In and Out address two
    // different connections (handled by two different threads).
    SECTION("getConnectionQosIn/Out distinguish the two directions of a bidirectional connection")
    {
        BufferedPort<Bottle> a;
        BufferedPort<Bottle> b;
        REQUIRE(a.open("/a"));
        REQUIRE(b.open("/b"));
        REQUIRE(NetworkBase::connect("/a", "/b"));
        REQUIRE(NetworkBase::connect("/b", "/a"));

        PortCoreClient aClient("/a");
        const return_getConnectionQos out = aClient.getConnectionQosOut("/b");
        const return_getConnectionQos in = aClient.getConnectionQosIn("/b");
        REQUIRE(out.found);
        REQUIRE(in.found);
        // the two connections are handled by two different threads
        CHECK(out.thread.tid != in.thread.tid);

#if defined(__linux__)
        // the type of service can be read back reliably only on linux
        REQUIRE(aClient.setConnectionQosOut("/b", -1, -1, 144).success);
        REQUIRE(aClient.setConnectionQosIn("/b", -1, -1, 40).success);
        CHECK(aClient.getConnectionQosOut("/b").qos_tos == 144);
        CHECK(aClient.getConnectionQosIn("/b").qos_tos == 40);
#endif

        b.close();
        a.close();
    }

    // The administrative protocol accepts only the numeric type of service: the
    // symbolic priorities (the levels NORM, LOW, HIGH, CRIT, the DSCP classes such
    // as AF12 or EF) are converted on the client side by yarp::os::QosStyle, as
    // done by yarp::os::Network::setConnectionQos, yarpmanager and the
    // "yarp priority-qos" command.
    SECTION("setConnectionQosIn/Out with the symbolic packet priorities (NORM, LOW, HIGH, CRIT, DSCP classes)")
    {
        Port sender;
        BufferedPort<Bottle> receiver;
        REQUIRE(sender.open("/send"));
        REQUIRE(receiver.open("/recv"));
        REQUIRE(NetworkBase::connect("/send", "/recv"));

        PortCoreClient sendClient("/send");

        // QosStyle::setPacketPriority() accepts "LEVEL:<level>", "DSCP:<class>"
        // and "TOS:<value>"; the TOS value is DSCP * 4
        struct
        {
            const char* priority;
            int tos;
        } priorities[] = {
            {"LEVEL:NORM", 0},    // NORMAL priority (DSCP 0)
            {"LEVEL:LOW", 40},    // LOW priority (DSCP 10, i.e. AF11)
            {"LEVEL:HIGH", 144},  // HIGH priority (DSCP 36, i.e. AF42)
            {"LEVEL:CRIT", 176},  // CRITICAL priority (DSCP 44, i.e. VA)
            {"DSCP:AF12", 48},    // assured forwarding class 1, drop precedence 2
            {"DSCP:EF", 184},     // expedited forwarding
            {"DSCP:CS1", 32},     // class selector 1
            {"TOS:72", 72},       // numeric value
        };

        for (const auto& p : priorities) {
            CAPTURE(p.priority);
            // conversion of the symbolic priority to the numeric type of service
            QosStyle style;
            REQUIRE(style.setPacketPriority(p.priority));
            REQUIRE(style.getPacketPriorityAsTOS() == p.tos);

            // the numeric value is sent with the administrative command,
            // leaving the scheduling of the thread unchanged (-1)
            const return_setConnectionQos result = sendClient.setConnectionQosOut("/recv", -1, -1, style.getPacketPriorityAsTOS());
            REQUIRE(result.ret);
            CHECK(result.success);
#if defined(__linux__)
            // the type of service can be read back reliably only on linux
            CHECK(sendClient.getConnectionQosOut("/recv").qos_tos == p.tos);
#endif
        }

        // the same through the higher level API, which configures both the
        // ends of the connection: HIGH on the sender side, NORMAL on the
        // receiver side
        QosStyle senderStyle;
        QosStyle receiverStyle;
        senderStyle.setPacketPriorityByLevel(QosStyle::PacketPriorityHigh);
        receiverStyle.setPacketPriorityByLevel(QosStyle::PacketPriorityNormal);
        CHECK(NetworkBase::setConnectionQos("/send", "/recv", senderStyle, receiverStyle, false));
#if defined(__linux__)
        CHECK(sendClient.getConnectionQosOut("/recv").qos_tos == 144);
        CHECK(PortCoreClient("/recv").getConnectionQosIn("/send").qos_tos == 0);
#endif

        receiver.close();
        sender.close();
    }

    // The scheduling of all the threads of the process which owns the port.
    SECTION("setProcessScheduling")
    {
        BufferedPort<Bottle> p1;
        REQUIRE(p1.open("/p1"));
#if defined(__linux__)
        // SCHED_OTHER with priority 0 is allowed without privileges
        CHECK(PortCoreClient("/p1").setProcessScheduling(0, 0));
#endif
        p1.close();
    }

    // Connections are added (with the default or with a given carrier) and
    // removed; adding a connection to a port which does not exist, or removing a
    // connection which does not exist, is reported as a failure.
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

    // addConnection reports the failure when the port does not accept the new
    // output (a read only port, or an rpc client which is already connected).
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

    // delConnection removes all the connections with the given port, whatever
    // their direction.
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

    // The client can also send the commands through an existing port in admin
    // mode, instead of opening a new connection for each command.
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

    // If the port cannot be reached, every method reports it (ret is false, or the
    // bool reply is false), instead of a meaningless reply.
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
        CHECK_FALSE(client.setPortmonitorParamsIn("(key 1)").ret);
        CHECK_FALSE(client.setPortmonitorParamsOut("(key 1)").ret);
        CHECK_FALSE(client.getPortmonitorParamsIn().ret);
        CHECK_FALSE(client.getPortmonitorParamsOut().ret);
        CHECK_FALSE(client.setConnectionParamsIn("/p1", "(key 1)").ret);
        CHECK_FALSE(client.setConnectionParamsOut("/p1", "(key 1)").ret);
        CHECK_FALSE(client.getConnectionParamsIn("/p1").ret);
        CHECK_FALSE(client.getConnectionParamsOut("/p1").ret);
        CHECK_FALSE(client.getPortInfo().ret);
        CHECK_FALSE(client.getConnectionQosIn("/p1").ret);
        CHECK_FALSE(client.getConnectionQosOut("/p1").ret);
        CHECK_FALSE(client.setConnectionQosIn("/p1", -1, -1, 0).ret);
        CHECK_FALSE(client.setConnectionQosOut("/p1", -1, -1, 0).ret);
        CHECK_FALSE(client.setProcessScheduling(0, 0));
    }

    NetworkBase::setLocalMode(false);
}

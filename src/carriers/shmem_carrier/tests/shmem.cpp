/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/Time.h>

#include <chrono>
#include <cstdlib>
#include <future>
#include <string>

#if !defined(_WIN32)
#  include <sys/wait.h>
#endif

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;

namespace {

// Replies to each incoming Bottle with the same Bottle, prefixed by "echo"
class EchoReader : public PortReader
{
public:
    bool read(ConnectionReader& connection) override
    {
        Bottle in;
        if (!in.read(connection)) {
            return false;
        }
        Bottle out;
        out.addString("echo");
        out.append(in);
        ConnectionWriter* writer = connection.getWriter();
        if (writer != nullptr) {
            out.write(*writer);
        }
        return true;
    }
};

// Wait until the connection is reported (or give up after a timeout)
bool waitConnection(const std::string& src, const std::string& dest, bool connected)
{
    for (int i = 0; i < 50; ++i) {
        if (Network::isConnected(src, dest, "shmem", true) == connected) {
            return true;
        }
        Time::delay(0.1);
    }
    return false;
}

// Run the shmem_peer helper in a different process, asynchronously.
// The result is the exit code of the helper.
std::future<int> runPeer(const std::string& mode, const Contact& rpc, const Contact& stream)
{
    std::string cmd = std::string("\"") + SHMEM_PEER_PATH + "\" " + mode + " " + rpc.getHost() + " "
                      + std::to_string(rpc.getPort()) + " " + std::to_string(stream.getPort());
#if defined(_WIN32)
    // cmd.exe strips the outer quotes
    cmd = "\"" + cmd + "\"";
#endif
    return std::async(std::launch::async, [cmd]() {
        int ret = std::system(cmd.c_str());
#if !defined(_WIN32)
        if (ret != -1 && WIFEXITED(ret)) {
            ret = WEXITSTATUS(ret);
        }
#endif
        return ret;
    });
}

} // namespace

TEST_CASE("carriers::shmem", "[carriers]")
{
    YARP_REQUIRE_PLUGIN("shmem", "carrier");

    Network::setLocalMode(true);

    SECTION("test small messages")
    {
        BufferedPort<Bottle> in;
        BufferedPort<Bottle> out;
        in.setStrict();

        REQUIRE(in.open("/shmem/in"));
        REQUIRE(out.open("/shmem/out"));
        REQUIRE(Network::connect(out.getName(), in.getName(), "shmem"));
        REQUIRE(waitConnection(out.getName(), in.getName(), true));

        for (int i = 0; i < 100; ++i) {
            Bottle& b = out.prepare();
            b.clear();
            b.addInt32(i);
            b.addString("hello");
            b.addFloat64(i * 0.5);
            out.writeStrict();

            Bottle* r = in.read();
            REQUIRE(r != nullptr);
            REQUIRE(r->size() == 3);
            CHECK(r->get(0).asInt32() == i);
            CHECK(r->get(1).asString() == "hello");
            CHECK(r->get(2).asFloat64() == i * 0.5);
        }

        out.interrupt();
        in.interrupt();
        out.close();
        in.close();
    }

    SECTION("test increasing message size (shared memory resize)")
    {
        BufferedPort<Bottle> in;
        BufferedPort<Bottle> out;
        in.setStrict();

        REQUIRE(in.open("/shmem/in"));
        REQUIRE(out.open("/shmem/out"));
        REQUIRE(Network::connect(out.getName(), in.getName(), "shmem"));
        REQUIRE(waitConnection(out.getName(), in.getName(), true));

        // The default buffer is SHMEM_DEFAULT_SIZE (4096) bytes, larger
        // messages force the shared memory to be reallocated
        for (size_t len : {10, 1000, 4000, 5000, 20000, 100000, 1000000}) {
            std::string payload(len, ' ');
            for (size_t j = 0; j < len; ++j) {
                payload[j] = static_cast<char>('a' + (j % 26));
            }

            Bottle& b = out.prepare();
            b.clear();
            b.addInt32(static_cast<int>(len));
            b.addString(payload);
            out.writeStrict();

            Bottle* r = in.read();
            REQUIRE(r != nullptr);
            REQUIRE(r->size() == 2);
            CHECK(r->get(0).asInt32() == static_cast<int>(len));
            CHECK(r->get(1).asString() == payload);
        }

        // Small messages must still work after the resize
        Bottle& b = out.prepare();
        b.clear();
        b.addString("after resize");
        out.writeStrict();
        Bottle* r = in.read();
        REQUIRE(r != nullptr);
        CHECK(r->get(0).asString() == "after resize");

        out.interrupt();
        in.interrupt();
        out.close();
        in.close();
    }

    SECTION("test disconnect and reconnect")
    {
        BufferedPort<Bottle> in;
        BufferedPort<Bottle> out;
        in.setStrict();

        REQUIRE(in.open("/shmem/in"));
        REQUIRE(out.open("/shmem/out"));

        for (int k = 0; k < 3; ++k) {
            REQUIRE(Network::connect(out.getName(), in.getName(), "shmem"));
            REQUIRE(waitConnection(out.getName(), in.getName(), true));

            Bottle& b = out.prepare();
            b.clear();
            b.addInt32(k);
            out.writeStrict();

            Bottle* r = in.read();
            REQUIRE(r != nullptr);
            CHECK(r->get(0).asInt32() == k);

            REQUIRE(Network::disconnect(out.getName(), in.getName()));
            REQUIRE(waitConnection(out.getName(), in.getName(), false));
        }

        out.interrupt();
        in.interrupt();
        out.close();
        in.close();
    }

    Network::setLocalMode(false);
}

TEST_CASE("carriers::shmem::rpc", "[carriers]")
{
    YARP_REQUIRE_PLUGIN("shmem", "carrier");

    Network::setLocalMode(true);

    Port server;
    Port client;
    EchoReader echo;
    server.setReader(echo);

    REQUIRE(server.open("/shmem/server"));
    REQUIRE(client.open("/shmem/client"));
    REQUIRE(Network::connect(client.getName(), server.getName(), "shmem"));
    REQUIRE(waitConnection(client.getName(), server.getName(), true));

    for (int i = 0; i < 20; ++i) {
        Bottle cmd;
        Bottle reply;
        cmd.addInt32(i);
        cmd.addString(std::string(static_cast<size_t>(i) * 1000, 'x'));
        REQUIRE(client.write(cmd, reply));
        REQUIRE(reply.size() == 3);
        CHECK(reply.get(0).asString() == "echo");
        CHECK(reply.get(1).asInt32() == i);
        CHECK(reply.get(2).asString().size() == static_cast<size_t>(i) * 1000);
    }

    client.close();
    server.close();

    Network::setLocalMode(false);
}

// The tests above run both sides of the connection in the same process. These
// ones run the other side in a different process (see shmem_peer.cpp), so
// that the inter-process objects (shared memory, semaphores) are really shared
// between two processes.
TEST_CASE("carriers::shmem::multiprocess", "[carriers]")
{
    YARP_REQUIRE_PLUGIN("shmem", "carrier");

    // The peer addresses the ports directly by host and port number, it does
    // not need to access the name server
    Network::setLocalMode(true);

    SECTION("rpc and streaming with another process")
    {
        Port server;
        EchoReader echo;
        server.setReader(echo);
        BufferedPort<Bottle> in;
        in.setStrict();

        REQUIRE(server.open("/shmem/xproc/server"));
        REQUIRE(in.open("/shmem/xproc/in"));

        auto peer = runPeer("normal", server.where(), in.where());

        // The peer performs the rpc calls (checking the replies) and then
        // streams 200 messages
        int received = 0;
        size_t bytes = 0;
        bool order_ok = true;
        double start = Time::now();
        while (received < 200 && Time::now() - start < 60.0) {
            Bottle* b = in.read(false);
            if (b == nullptr) {
                if (peer.wait_for(std::chrono::seconds(0)) == std::future_status::ready && in.getPendingReads() == 0) {
                    break;
                }
                Time::delay(0.01);
                continue;
            }
            if (b->get(0).asInt32() != received) {
                order_ok = false;
            }
            bytes += b->get(1).asString().size();
            ++received;
        }

        CHECK(peer.get() == 0);
        CHECK(received == 200);
        CHECK(order_ok);
        CHECK(bytes == 196 * 100 + 4 * 500000);

        in.close();
        server.close();
    }

    SECTION("the other process crashes")
    {
        BufferedPort<Bottle> in;
        in.setStrict();
        REQUIRE(in.open("/shmem/xproc/in"));

        auto peer = runPeer("crash", in.where(), in.where());

        Bottle* b = in.read();
        REQUIRE(b != nullptr);
        CHECK(b->get(0).asString() == "bye");
        CHECK(peer.get() == 0);

        // The peer exited without closing the connection, this must be
        // detected and the connection removed
        bool removed = false;
        for (int i = 0; i < 100 && !removed; ++i) {
            removed = (in.getInputCount() == 0);
            Time::delay(0.1);
        }
        CHECK(removed);

        in.close();
    }

    Network::setLocalMode(false);
}

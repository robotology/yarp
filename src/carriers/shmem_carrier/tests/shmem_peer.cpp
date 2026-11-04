/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Helper process for the multi-process shmem tests (see shmem.cpp).
 *
 * Usage:
 *   shmem_peer <mode> <host> <rpc_port> <stream_port>
 *
 * It connects to the ports opened by the test process using the shmem
 * carrier, bypassing the name server (the ports are addressed directly by
 * host and port number).
 *
 * Parameters:
 *   <mode>         what the peer does after connecting:
 *                    normal: connects to both ports, performs rpc calls on
 *                            <rpc_port> (checking each reply), streams
 *                            messages to <stream_port>, then closes cleanly
 *                    crash:  connects only to <stream_port>, sends one
 *                            message, then exits without closing anything,
 *                            as if the process crashed
 *   <host>         host name or IP address of the machine where the test
 *                  process opened its ports (the host part of
 *                  Contact::where(), e.g. "localhost")
 *   <rpc_port>     TCP port number of the test's rpc server port, i.e. a
 *                  yarp::os::Port with a reader that replies to every
 *                  message with "echo" followed by the message itself.
 *                  Ignored in crash mode (any value is accepted)
 *   <stream_port>  TCP port number of the test's input port, i.e. a
 *                  yarp::os::BufferedPort<Bottle> receiving the streamed
 *                  messages
 *
 * Exit code:
 *   0  success
 *   1  wrong command line arguments
 *   2  the ports could not be opened or connected
 *   3  at least one rpc call or stream write failed (normal mode only)
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>

#include <cstdio>
#include <cstdlib>
#include <string>

using namespace yarp::os;

namespace {

constexpr int rpc_count = 30;
constexpr int stream_count = 200;

size_t rpcPayloadSize(int i)
{
    // Every 10 messages send a big one, to force the reallocation of the
    // shared memory in both directions
    return (i % 10 == 9) ? 2000000 : static_cast<size_t>(i) * 1000;
}

size_t streamPayloadSize(int i)
{
    return (i % 50 == 49) ? 500000 : 100;
}

bool connectTo(Contactable& port, const std::string& host, int portnum)
{
    Contact dest("/shmem/xproc/remote", "shmem", host, portnum);
    for (int i = 0; i < 20; ++i) {
        if (port.addOutput(dest)) {
            return true;
        }
        Time::delay(0.1);
    }
    return false;
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc != 5) {
        std::fprintf(stderr, "Usage: %s <normal|crash> <host> <rpc_port> <stream_port>\n", argv[0]);
        return 1;
    }

    const std::string mode = argv[1];
    const std::string host = argv[2];
    const int rpc_port = std::atoi(argv[3]);
    const int stream_port = std::atoi(argv[4]);

    Network yarp;
    // Our ports are only used for output and are not reachable from the
    // network (openFake), the remote ones are addressed directly, therefore no
    // name server is needed
    Network::setLocalMode(true);

    if (mode == "crash") {
        Port out;
        if (!out.openFake("/shmem/xproc/peer/out") || !connectTo(out, host, stream_port)) {
            std::fprintf(stderr, "PEER: cannot connect\n");
            return 2;
        }
        Bottle b;
        b.addString("bye");
        out.write(b);
        Time::delay(0.5);
        // Exit without closing the ports, as if the process crashed
        std::fflush(stdout);
        std::_Exit(0);
    }

    if (mode != "normal") {
        std::fprintf(stderr, "PEER: unknown mode %s\n", mode.c_str());
        return 1;
    }

    Port client;
    Port out;
    if (!client.openFake("/shmem/xproc/peer/client") || !out.openFake("/shmem/xproc/peer/out")) {
        std::fprintf(stderr, "PEER: cannot open ports\n");
        return 2;
    }
    if (!connectTo(client, host, rpc_port) || !connectTo(out, host, stream_port)) {
        std::fprintf(stderr, "PEER: cannot connect\n");
        return 2;
    }

    int failures = 0;

    for (int i = 0; i < rpc_count; ++i) {
        const size_t len = rpcPayloadSize(i);
        Bottle cmd;
        Bottle reply;
        cmd.addInt32(i);
        cmd.addString(std::string(len, static_cast<char>('a' + i % 26)));
        if (!client.write(cmd, reply)
            || reply.size() != 3
            || reply.get(0).asString() != "echo"
            || reply.get(1).asInt32() != i
            || reply.get(2).asString() != cmd.get(1).asString()) {
            std::fprintf(stderr, "PEER: rpc %d failed\n", i);
            ++failures;
        }
    }

    for (int i = 0; i < stream_count; ++i) {
        Bottle b;
        b.addInt32(i);
        b.addString(std::string(streamPayloadSize(i), 'z'));
        if (!out.write(b)) {
            std::fprintf(stderr, "PEER: stream write %d failed\n", i);
            ++failures;
        }
    }

    // Tell the test that the stream is over
    Bottle cmd;
    Bottle reply;
    cmd.addString("done");
    if (!client.write(cmd, reply)) {
        std::fprintf(stderr, "PEER: final rpc failed\n");
        ++failures;
    }

    out.close();
    client.close();

    std::printf("PEER: %d failures\n", failures);
    return failures == 0 ? 0 : 3;
}

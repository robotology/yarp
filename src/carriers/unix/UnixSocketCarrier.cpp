/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "UnixSocketCarrier.h"

#include <yarp/conf/environment.h>
#include <yarp/conf/filesystem.h>

#include <yarp/os/ConnectionState.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Os.h>

#include "UnixSocketLogComponent.h"

#include <array>
#include <mutex>

using namespace yarp::os;
namespace fs = yarp::conf::filesystem;

namespace {

// FIXME: This method should be available somewhere in YARP
std::string getYARPRuntimeDir()
{
    static std::mutex m;
    std::lock_guard<std::mutex> lock(m);

    static std::string yarp_runtime_dir;
    bool found = false;

    // If already populated, there is nothing to do
    if (!yarp_runtime_dir.empty()) {
        return yarp_runtime_dir;
    }

    // Check YARP_RUNTIME_DIR
    yarp_runtime_dir = yarp::conf::environment::get_string("YARP_RUNTIME_DIR", &found);
    if (found) {
        return yarp_runtime_dir;
    }

    // Check XDG_RUNTIME_DIR
    std::string xdg_runtime_dir = yarp::conf::environment::get_string("XDG_RUNTIME_DIR", &found);
    if (found) {
        yarp_runtime_dir = xdg_runtime_dir + fs::preferred_separator + "yarp";
        return yarp_runtime_dir;
    }

    // Use /tmp/runtime-user
    std::string user = yarp::conf::environment::get_string("USER", &found);
    if (found) {
        yarp_runtime_dir = "/tmp/runtime-" + user + fs::preferred_separator + "yarp";
        return yarp_runtime_dir;
    }

    // ERROR
    return {};
}

/**
 * @brief isUnixSockSupported
 * @param proto, contains the information of the connection
 * @return true if the remote and the local port are on the same host
 */
bool isUnixSockSupported(ConnectionState& proto) // FIXME Why is this method unused?
{
    yarp::os::Contact remote = proto.getStreams().getRemoteAddress();
    yarp::os::Contact local = proto.getStreams().getLocalAddress();

    if (remote.getHost() != local.getHost()) {
        yCError(UNIXSOCK_CARRIER,
                "The ports are on different machines, unix socket not supported...");
        return false;
    }
    return true;
}

} // namespace

yarp::os::Carrier* UnixSocketCarrier::create() const
{
    return new UnixSocketCarrier();
}

std::string UnixSocketCarrier::getName() const
{
    return name;
}

bool UnixSocketCarrier::requireAck() const
{
    return requireAckFlag;
}

bool UnixSocketCarrier::isConnectionless() const
{
    return false;
}

bool UnixSocketCarrier::checkHeader(const Bytes& header)
{
    if (header.length() != headerSize) {
        return false;
    }

    bool isUnix = true;
    bool isUnix_ack = true;

    const char* target = headerCode;
    const char* target_ack = headerCode_ack;
    for (size_t i = 0; i < headerSize; i++) {
        if (header.get()[i] != target[i]) {
            isUnix = false;
        }
        if (header.get()[i] != target_ack[i]) {
            isUnix_ack = false;
        }
    }

    return (isUnix || isUnix_ack);
}

void UnixSocketCarrier::getHeader(Bytes& header) const
{
    const char* target = requireAckFlag ? headerCode_ack : headerCode;
    for (size_t i = 0; i < headerSize && i < header.length(); i++) {
        header.get()[i] = target[i];
    }
}

bool UnixSocketCarrier::sendIndex(ConnectionState& proto, SizedWriter& writer)
{
    YARP_UNUSED(proto);
    YARP_UNUSED(writer);

    return true;
}

bool UnixSocketCarrier::expectIndex(ConnectionState& proto)
{
    YARP_UNUSED(proto);

    return true;
}

bool UnixSocketCarrier::sendAck(ConnectionState& proto)
{
    if (requireAckFlag) {
        const Bytes ack_bytes(const_cast<char*>(ack_string), ack_string_size);
        proto.os().write(ack_bytes);
    }
    return true;
}

bool UnixSocketCarrier::expectAck(ConnectionState& proto)
{
    if (requireAckFlag) {
        std::array<char, ack_string_size> buf;
        Bytes ack(buf.data(), buf.size());
        yarp::conf::ssize_t hdr = proto.is().readFull(ack);
        if (static_cast<size_t>(hdr) != ack.length()) {
            yCDebug(UNIXSOCK_CARRIER, "Did not get ack");
            return false;
        }

        const char* target = ack_string;
        for (size_t i = 0; i < ack_string_size; i++) {
            if (ack.get()[i] != target[i]) {
                yCDebug(UNIXSOCK_CARRIER, "Bad ack");
                return false;
            }
        }
    }
    return true;
}

bool UnixSocketCarrier::respondToHeader(ConnectionState& proto)
{
    // I am the receiver
    return becomeUnixSocket(proto, false);
}

bool UnixSocketCarrier::expectReplyToHeader(ConnectionState& proto)
{
    // I am the sender
    return becomeUnixSocket(proto, true);
}

bool UnixSocketCarrier::becomeUnixSocket(ConnectionState& proto, bool sender)
{
    if (!isUnixSockSupported(proto)) {
        return false;
    }

    Contact remote = proto.getStreams().getRemoteAddress();
    Contact local = proto.getStreams().getLocalAddress();

    proto.takeStreams(nullptr); // free up port from tcp

    std::string runtime_dir = getYARPRuntimeDir();

    // Make sure that the path exists
    if (runtime_dir.empty() || yarp::os::mkdir_p(runtime_dir.c_str(), 0) != 0) {
        yCError(UNIXSOCK_CARRIER, "Failed to create directory %s", runtime_dir.c_str());
        return false;
    }

    if (sender) {
        socketPath = runtime_dir + fs::preferred_separator + std::to_string(remote.getPort()) + "_" + std::to_string(local.getPort()) + ".sock";
    } else {
        socketPath = runtime_dir + fs::preferred_separator + std::to_string(local.getPort()) + "_" + std::to_string(remote.getPort()) + ".sock";
    }

    stream = new UnixSockTwoWayStream(socketPath);
    stream->setLocalAddress(local);
    stream->setRemoteAddress(remote);

    if (!stream->open(sender)) {
        delete stream;
        stream = nullptr;
        yCError(UNIXSOCK_CARRIER, "Failed to open stream on socket %s as %s", socketPath.c_str(), (sender ? "sender" : "receiver"));
        return false;
    }
    yAssert(stream != nullptr);

    proto.takeStreams(stream);

    yCDebug(UNIXSOCK_CARRIER, "Connected on socket %s as %s", socketPath.c_str(), (sender ? "sender" : "receiver"));
    return true;
}


bool UnixSocketCarrier::configure(yarp::os::ConnectionState& proto)
{
    Property options;
    options.fromString(proto.getSenderSpecifier());
    return configureFromProperty(options);
}

bool UnixSocketCarrier::configureFromProperty(yarp::os::Property& options)
{
    if (options.check("ack")) {
        yCInfo(UNIXSOCK_CARRIER, "ACK Enabled");
        requireAckFlag = true;
    }
    return true;
}

void UnixSocketCarrier::setParameters(const yarp::os::Bytes& header)
{
    const char* target_ack = headerCode_ack;
    for (size_t i = 0; i < headerSize; i++) {
        if (header.get()[i] != target_ack[i]) {
            return;
        }
    }
    yCInfo(UNIXSOCK_CARRIER, "ACK Enabled");
    requireAckFlag = true;
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ShmemHybridStream.h"
#include "ShmemLogComponent.h"

#include <yarp/os/Log.h>

namespace {
yarp::os::Contact toContact(bool ok, const std::string& host, int port)
{
    return ok ? yarp::os::Contact(host, port) : yarp::os::Contact();
}
} // namespace

ShmemHybridStream::ShmemHybridStream() :
        m_bLinked(false)
{
}

ShmemHybridStream::~ShmemHybridStream()
{
    close();
}

int ShmemHybridStream::open(const yarp::os::Contact& yarp_address, bool sender)
{
    m_bLinked = false;

    if (sender) {
        return connect(yarp_address);
    }

    // The receiver listens on any interface
    if (!m_Acceptor.listen(yarp_address.getPort())) {
        yCError(SHMEMCARRIER, "ShmemHybridStream can't listen on port %d", yarp_address.getPort());
        return -1;
    }

    std::string host;
    int port = 0;
    bool ok = m_Acceptor.getLocalAddress(host, port);
    m_LocalAddress = toContact(ok, host, port);
    m_RemoteAddress = m_LocalAddress; // finalized in call to accept()

    return 0;
}

int ShmemHybridStream::accept()
{
    if (m_bLinked) {
        return -1;
    }

    bool ok = m_Acceptor.accept(m_Socket);
    // Only one connection is accepted
    m_Acceptor.close();

    if (!ok) {
        yCError(SHMEMCARRIER, "ShmemHybridStream accept failed");
        close();
        return -1;
    }

    std::string host;
    int port = 0;
    ok = m_Socket.getLocalAddress(host, port);
    m_LocalAddress = toContact(ok, host, port);
    ok = m_Socket.getRemoteAddress(host, port);
    m_RemoteAddress = toContact(ok, host, port);

    ShmemPacket_t recv_conn_data;
    if (!m_Socket.recvAll(&recv_conn_data, sizeof recv_conn_data)) {
        yCError(SHMEMCARRIER, "ShmemHybridStream socket reading error");
        close();
        return -1;
    }

    if (!in.open(m_RemoteAddress.getPort(), &m_Socket)) {
        yCError(SHMEMCARRIER, "ShmemHybridStream can't open shared memory");
        close();
        return -1;
    }

    if (!out.open(m_LocalAddress.getPort())) {
        yCError(SHMEMCARRIER, "ShmemHybridStream can't create shared memory");
        close();
        return -1;
    }

    ShmemPacket_t send_conn_data;
    send_conn_data.command = ACKNOWLEDGE;
    send_conn_data.size = 0;
    if (!m_Socket.sendAll(&send_conn_data, sizeof send_conn_data)) {
        yCError(SHMEMCARRIER, "ShmemHybridStream socket writing error");
        close();
        return -1;
    }

    m_bLinked = true;

    m_Socket.setNonBlocking();

    return 0;
}

int ShmemHybridStream::connect(const yarp::os::Contact& address)
{
    if (m_bLinked) {
        return -1;
    }

    if (!m_Socket.connect(address.getHost(), address.getPort())) {
        yCError(SHMEMCARRIER, "ShmemHybridStream can't connect to %s:%d", address.getHost().c_str(), address.getPort());
        close();
        return -1;
    }

    std::string host;
    int port = 0;
    bool ok = m_Socket.getLocalAddress(host, port);
    m_LocalAddress = toContact(ok, host, port);
    ok = m_Socket.getRemoteAddress(host, port);
    m_RemoteAddress = toContact(ok, host, port);

    if (!out.open(m_LocalAddress.getPort())) {
        yCError(SHMEMCARRIER, "ShmemHybridStream can't create shared memory");
        close();
        return -1;
    }

    ShmemPacket_t send_conn_data;
    send_conn_data.command = CONNECT;
    send_conn_data.size = SHMEM_DEFAULT_SIZE;
    if (!m_Socket.sendAll(&send_conn_data, sizeof send_conn_data)) {
        yCError(SHMEMCARRIER, "ShmemHybridStream socket writing error");
        close();
        return -1;
    }

    ShmemPacket_t recv_conn_data;
    if (!m_Socket.recvAll(&recv_conn_data, sizeof recv_conn_data)) {
        yCError(SHMEMCARRIER, "ShmemHybridStream socket reading error");
        close();
        return -1;
    }

    if (!in.open(m_RemoteAddress.getPort(), &m_Socket)) {
        yCError(SHMEMCARRIER, "ShmemHybridStream can't open shared memory");
        close();
        return -1;
    }

    m_bLinked = true;

    m_Socket.setNonBlocking();

    return 0;
}

void ShmemHybridStream::close()
{
    m_bLinked = false;
    in.close();
    out.close();
    m_Socket.close();
    m_Acceptor.close();
}

void ShmemHybridStream::interrupt()
{
    yCDebug(SHMEMCARRIER, "INTERRUPT");
    close();
}

void ShmemHybridStream::write(const yarp::os::Bytes& b)
{
    if (!out.write(b)) {
        close();
    }
}

yarp::conf::ssize_t ShmemHybridStream::read(yarp::os::Bytes& b)
{
    yarp::conf::ssize_t ret = in.read(b);
    if (ret == -1) {
        close();
    }
    return ret;
}

yarp::os::InputStream& ShmemHybridStream::getInputStream()
{
    return *this;
}

yarp::os::OutputStream& ShmemHybridStream::getOutputStream()
{
    return *this;
}

bool ShmemHybridStream::isOk() const
{
    return m_bLinked && in.isOk() && out.isOk();
}

void ShmemHybridStream::reset()
{
    yCDebug(SHMEMCARRIER, "RECEIVED RESET COMMAND");
    close();
}

void ShmemHybridStream::beginPacket()
{
}

void ShmemHybridStream::endPacket()
{
}

const yarp::os::Contact& ShmemHybridStream::getLocalAddress() const
{
    return m_LocalAddress;
}

const yarp::os::Contact& ShmemHybridStream::getRemoteAddress() const
{
    return m_RemoteAddress;
}

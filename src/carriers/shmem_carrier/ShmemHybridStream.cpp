/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ShmemHybridStream.h"
#include "ShmemLogComponent.h"

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

    ACE_INET_Addr ace_address(yarp_address.getPort(), yarp_address.getHost().c_str());

    if (sender) {
        return connect(ace_address);
    } else {
        ACE_INET_Addr ace_server_addr(ace_address.get_port_number());

        int result = m_Acceptor.open(ace_server_addr);

        if (result < 0) {
            yCError(SHMEMCARRIER, "ShmemHybridStream open result %d", result);
            return result;
        }

        m_Acceptor.get_local_addr(ace_server_addr);

        m_LocalAddress = yarp::os::Contact(ace_server_addr.get_host_addr(), ace_server_addr.get_port_number());
        m_RemoteAddress = m_LocalAddress; // finalized in call to accept()

        return result;
    }

    return 1;
}

int ShmemHybridStream::accept()
{
    if (m_bLinked) {
        return -1;
    }

    yarp::conf::ssize_t result = m_Acceptor.accept(m_SockStream);

    if (result < 0) {
        yCError(SHMEMCARRIER, "ShmemHybridStream server returned %zd", result);
        close();
        return -1;
    }

    ACE_INET_Addr local, remote;
    m_SockStream.get_local_addr(local);
    m_SockStream.get_remote_addr(remote);
    m_LocalAddress = yarp::os::Contact(local.get_host_addr(), local.get_port_number());
    m_RemoteAddress = yarp::os::Contact(remote.get_host_addr(), remote.get_port_number());

    ShmemPacket_t recv_conn_data;
    result = m_SockStream.recv_n(&recv_conn_data, sizeof recv_conn_data);
    if (result <= 0) {
        yCError(SHMEMCARRIER, "Socket returned %zd", result);
        close();
        return -1;
    }

    if (!in.open(m_RemoteAddress.getPort(), &m_SockStream)) {
        yCError(SHMEMCARRIER, "ShmemHybridStream can't create shared memory");
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
    if (m_SockStream.send_n(&send_conn_data, sizeof send_conn_data) <= 0) {
        yCError(SHMEMCARRIER, "ShmemHybridStream socket writing error");
        close();
        return -1;
    }

    m_bLinked = true;

    m_SockStream.enable(ACE_NONBLOCK);

    return 0;
}

int ShmemHybridStream::connect(const ACE_INET_Addr& ace_address)
{
    if (m_bLinked) {
        return -1;
    }

    ACE_SOCK_Connector connector;
    yarp::conf::ssize_t result = connector.connect(m_SockStream, ace_address);
    if (result < 0) {
        yCError(SHMEMCARRIER, "ShmemHybridStream client returned %zd", result);
        close();
        return -1;
    }

    ACE_INET_Addr local, remote;
    m_SockStream.get_local_addr(local);
    m_SockStream.get_remote_addr(remote);
    m_LocalAddress = yarp::os::Contact(local.get_host_addr(), local.get_port_number());
    m_RemoteAddress = yarp::os::Contact(remote.get_host_addr(), remote.get_port_number());

    out.open(m_LocalAddress.getPort());

    ShmemPacket_t send_conn_data;
    send_conn_data.command = CONNECT;
    send_conn_data.size = SHMEM_DEFAULT_SIZE;
    result = m_SockStream.send_n(&send_conn_data, sizeof send_conn_data);
    if (result <= 0) {
        yCError(SHMEMCARRIER, "Socket returned %zd", result);
        close();
        return -1;
    }

    ShmemPacket_t recv_conn_data;
    result = m_SockStream.recv_n(&recv_conn_data, sizeof recv_conn_data);
    if (result <= 0) {
        yCError(SHMEMCARRIER, "Socket returned %zd", result);
        close();
        return -1;
    }

    in.open(m_RemoteAddress.getPort(), &m_SockStream);

    m_bLinked = true;

    m_SockStream.enable(ACE_NONBLOCK);

    return 0;
}

void ShmemHybridStream::close()
{
    m_bLinked = false;
    in.close();
    out.close();
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

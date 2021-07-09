/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/DgramTwoWayStream.h>

#include <yarp/conf/system.h>
#include <yarp/conf/environment.h>

#include <yarp/os/NetType.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/LogComponent.h>

#if defined(YARP_HAS_ACE)
#    include <ace/ACE.h>
#    include <ace/Handle_Set.h>
#    include <ace/INET_Addr.h>
#    include <ace/Log_Msg.h>
#    include <ace/OS_Memory.h>
#    include <ace/OS_NS_sys_select.h>
#    include <ace/SOCK_Dgram.h>
#    include <ace/SOCK_Dgram_Mcast.h>
#    include <ace/os_include/net/os_if.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#else
#    include <arpa/inet.h>
#    include <netinet/in.h>
#    include <sys/socket.h>
#    include <sys/types.h>
#    include <unistd.h>
#endif

#include <cerrno>
#include <cstring>

using namespace yarp::os::impl;
using namespace yarp::os;

#define CRC_SIZE 8
#define UDP_MAX_DATAGRAM_SIZE (65507 - CRC_SIZE)


namespace {
YARP_OS_LOG_COMPONENT(DGRAMTWOWAYSTREAM, "yarp.os.impl.DgramTwoWayStream")
} // namespace


static bool checkCrc(char* buf, yarp::conf::ssize_t length, yarp::conf::ssize_t crcLength, int pct, int* store_altPct = nullptr)
{
    auto alt = (NetInt32)NetType::getCrc(buf + crcLength,
                                         (length > crcLength) ? (length - crcLength) : 0);
    Bytes b(buf, 4);
    Bytes b2(buf + 4, 4);
    NetInt32 curr = NetType::netInt(b);
    int altPct = NetType::netInt(b2);
    bool ok = (alt == curr && pct == altPct);
    if (!ok) {
        if (alt != curr) {
            yCDebug(DGRAMTWOWAYSTREAM, "crc mismatch");
        }
        if (pct != altPct) {
            yCDebug(DGRAMTWOWAYSTREAM, "packet code broken");
        }
    }
    if (store_altPct != nullptr) {
        *store_altPct = altPct;
    }

    return ok;
}


static void addCrc(char* buf, yarp::conf::ssize_t length, yarp::conf::ssize_t crcLength, int pct)
{
    auto alt = (NetInt32)NetType::getCrc(buf + crcLength,
                                         (length > crcLength) ? (length - crcLength) : 0);
    Bytes b(buf, 4);
    Bytes b2(buf + 4, 4);
    NetType::netInt((NetInt32)alt, b);
    NetType::netInt((NetInt32)pct, b2);
}


bool DgramTwoWayStream::open(const Contact& remote)
{
#if defined(YARP_HAS_ACE)
    ACE_INET_Addr anywhere((u_short)0, (ACE_UINT32)INADDR_ANY);
    Contact local(anywhere.get_host_addr(),
                  anywhere.get_port_number());
#else
    Contact local("localhost", -1);
#endif
    return open(local, remote);
}

bool DgramTwoWayStream::open(const Contact& local, const Contact& remote)
{
    localAddress = local;
    remoteAddress = remote;

#if defined(YARP_HAS_ACE)
    localHandle = ACE_INET_Addr((u_short)(localAddress.getPort()), (ACE_UINT32)INADDR_ANY);
    if (remote.isValid()) {
        remoteHandle.set(remoteAddress.getPort(), remoteAddress.getHost().c_str());
    }
    dgram = new ACE_SOCK_Dgram;
    yCAssert(DGRAMTWOWAYSTREAM, dgram != nullptr);

    int result = dgram->open(localHandle,
                             ACE_PROTOCOL_FAMILY_INET,
                             0,
                             1);
#else
    dgram = nullptr;
    dgram_sockfd = -1;

    int s = -1;
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        std::exit(1);
    }
    struct sockaddr_in dgram_sin;
    memset((char*)&dgram_sin, 0, sizeof(dgram_sin));
    dgram_sin.sin_family = AF_INET;
    dgram_sin.sin_addr.s_addr = htonl(INADDR_ANY);
    dgram_sin.sin_port = htons(remote.getPort());
    if (local.isValid()) {
        if (inet_pton(AF_INET, remote.getHost().c_str(), &dgram_sin.sin_addr) == 0) {
            yCError(DGRAMTWOWAYSTREAM, "could not set up udp client");
            std::exit(1);
        }
        if (connect(s, (struct sockaddr*)&dgram_sin, sizeof(dgram_sin)) == -1) {
            yCError(DGRAMTWOWAYSTREAM, "could not connect udp client");
            std::exit(1);
        }
    } else {
        if (bind(s, (struct sockaddr*)&dgram_sin, sizeof(dgram_sin)) == -1) {
            yCError(DGRAMTWOWAYSTREAM, "could not create udp server");
            std::exit(1);
        }
    }
    dgram_sockfd = s;
    dgram = this;
    int result = -1;
    int local_port = -1;

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(dgram_sockfd, (struct sockaddr*)&sin, &len) == 0 && sin.sin_family == AF_INET) {
        result = 0;
        local_port = ntohs(sin.sin_port);
    }
#endif

    if (result != 0) {
        yCError(DGRAMTWOWAYSTREAM, "could not open datagram socket");
        return false;
    }

    configureSystemBuffers();

#if defined(YARP_HAS_ACE)
    dgram->get_local_addr(localHandle);
    yCDebug(DGRAMTWOWAYSTREAM, "starting DGRAM entity on port number %u",localHandle.get_port_number());
    localAddress = Contact("127.0.0.1", localHandle.get_port_number());
#else
    localAddress = Contact("127.0.0.1", local_port);
#endif

    yCDebug(DGRAMTWOWAYSTREAM, "Update: DGRAM from %s to %s", localAddress.toURI().c_str(), remoteAddress.toURI().c_str());

    allocate();

    return true;
}

void DgramTwoWayStream::allocate(int readSize, int writeSize)
{
    //These are only as another default. We should modify the method to return bool
    //and fail if we cannot read the socket size.

    int _read_size = -1;
    int _write_size = -1;

    std::string _env_dgram = yarp::conf::environment::get_string("YARP_DGRAM_SIZE");
    std::string _env_mode;
    if (multiMode) {
        _env_mode = yarp::conf::environment::get_string("YARP_MCAST_SIZE");
    } else {
        _env_mode = yarp::conf::environment::get_string("YARP_UDP_SIZE");
    }
    if (!_env_mode.empty()) {
        _env_dgram = _env_mode;
    }
    if (!_env_dgram.empty()) {
        int sz = yarp::conf::numeric::from_string<int>(_env_dgram);
        if (sz != 0) {
            _read_size = _write_size = sz;
        }
        yCInfo(DGRAMTWOWAYSTREAM, "Datagram packet size set to %d", _read_size);
    }
    if (readSize != 0) {
        _read_size = readSize;
        yCInfo(DGRAMTWOWAYSTREAM, "Datagram read size reset to %d", _read_size);
    }
    if (writeSize != 0) {
        _write_size = writeSize;
        yCInfo(DGRAMTWOWAYSTREAM, "Datagram write size reset to %d", _write_size);
    }

    // force the size of the write buffer to be under the max size of a udp datagram.
    if (_write_size > UDP_MAX_DATAGRAM_SIZE || _write_size < 0) {
        _write_size = UDP_MAX_DATAGRAM_SIZE;
    }

    if (_read_size < 0) {
#if defined(YARP_HAS_ACE)
        //Defaults to socket size
        if (dgram != nullptr) {
            int len = sizeof(_read_size);
            int result = dgram->get_option(SOL_SOCKET, SO_RCVBUF, &_read_size, &len);
            if (result < 0) {
                yCError(DGRAMTWOWAYSTREAM, "Failed to read buffer size from RCVBUF socket with error: %s. Setting read buffer size to UDP_MAX_DATAGRAM_SIZE.",
                           strerror(errno));
                _read_size = UDP_MAX_DATAGRAM_SIZE;
            }
        }
#else
        socklen_t len = sizeof(_read_size);

        int result = getsockopt(dgram_sockfd, SOL_SOCKET, SO_RCVBUF, &_read_size, &len);
        if (result < 0) {
            yCError(DGRAMTWOWAYSTREAM, "Failed to read buffer size from RCVBUF socket with error: %s. Setting read buffer size to UDP_MAX_DATAGRAM_SIZE.",
                       strerror(errno));
            _read_size = UDP_MAX_DATAGRAM_SIZE;
        }
#endif
    }

    readBuffer.allocate(_read_size);
    writeBuffer.allocate(_write_size);
    readAt = 0;
    readAvail = 0;
    writeAvail = CRC_SIZE;
    //happy = true;
    pct = 0;
}


void DgramTwoWayStream::configureSystemBuffers()
{
    //By default the buffers are forced to the datagram size limit.
    //These can be overwritten by environment variables
    //Generic variable
    std::string socketBufferSize = yarp::conf::environment::get_string("YARP_DGRAM_BUFFER_SIZE");
    //Specific read
    std::string socketReadBufferSize = yarp::conf::environment::get_string("YARP_DGRAM_RECV_BUFFER_SIZE");
    //Specific write
    std::string socketSendBufferSize = yarp::conf::environment::get_string("YARP_DGRAM_SND_BUFFER_SIZE");

    int readBufferSize = -1;
    if (!socketReadBufferSize.empty()) {
        readBufferSize = yarp::conf::numeric::from_string<int>(socketReadBufferSize);
    } else if (!socketBufferSize.empty()) {
        readBufferSize = yarp::conf::numeric::from_string<int>(socketBufferSize);
    }

    int writeBufferSize = -1;
    if (!socketSendBufferSize.empty()) {
        writeBufferSize = yarp::conf::numeric::from_string<int>(socketSendBufferSize);
    } else if (!socketBufferSize.empty()) {
        writeBufferSize = yarp::conf::numeric::from_string<int>(socketBufferSize);
    }
    // The writeBufferSize can't be set greater than udp datagram
    // maximum size
    if (writeBufferSize < 0 || writeBufferSize > UDP_MAX_DATAGRAM_SIZE) {
        if (writeBufferSize > UDP_MAX_DATAGRAM_SIZE) {
            yCWarning(DGRAMTWOWAYSTREAM, "The desired SND buffer size is too big. It is set to the max datagram size : %d",
                         UDP_MAX_DATAGRAM_SIZE);
        }
        writeBufferSize = UDP_MAX_DATAGRAM_SIZE;
    }

    if (readBufferSize > 0) {
        int actualReadSize = -1;

#if defined(YARP_HAS_ACE)
        int intSize = sizeof(readBufferSize);
        int setResult = dgram->set_option(SOL_SOCKET, SO_RCVBUF, (void*)&readBufferSize, intSize);

        int getResult = dgram->get_option(SOL_SOCKET, SO_RCVBUF, (void*)&actualReadSize, &intSize);
#else
        socklen_t intSize = sizeof(readBufferSize);
        int setResult = setsockopt(dgram_sockfd, SOL_SOCKET, SO_RCVBUF, (void*)&readBufferSize, intSize);
        int getResult = getsockopt(dgram_sockfd, SOL_SOCKET, SO_RCVBUF, (void*)&actualReadSize, &intSize);
#endif
        // in linux the value returned by getsockopt is "doubled"
        // for some unknown reasons (see https://linux.die.net/man/7/socket)
#if defined(__linux__)
        actualReadSize /= 2;
#endif
        if (setResult < 0 || getResult < 0 || readBufferSize != actualReadSize) {
            bufferAlertNeeded = true;
            bufferAlerted = false;
            yCWarning(DGRAMTWOWAYSTREAM, "Failed to set RECV socket buffer to desired size. Actual: %d, Desired %d",
                         actualReadSize,
                         readBufferSize);
        }
    }
    if (writeBufferSize > 0) {
        int actualWriteSize = -1;
#if defined(YARP_HAS_ACE)
        int intSize = sizeof(writeBufferSize);
        int setResult = dgram->set_option(SOL_SOCKET, SO_SNDBUF, (void*)&writeBufferSize, intSize);
        int getResult = dgram->get_option(SOL_SOCKET, SO_SNDBUF, (void*)&actualWriteSize, &intSize);
#else
        socklen_t intSize = sizeof(writeBufferSize);
        int setResult = setsockopt(dgram_sockfd, SOL_SOCKET, SO_SNDBUF, (void*)&writeBufferSize, intSize);
        int getResult = getsockopt(dgram_sockfd, SOL_SOCKET, SO_SNDBUF, (void*)&actualWriteSize, &intSize);
#endif
        // in linux the value returned by getsockopt is "doubled"
        // for some unknown reasons (see https://linux.die.net/man/7/socket)
#if defined(__linux__)
        actualWriteSize /= 2;
#endif
        if (setResult < 0 || getResult < 0 || writeBufferSize != actualWriteSize) {
            bufferAlertNeeded = true;
            bufferAlerted = false;
            yCWarning(DGRAMTWOWAYSTREAM, "Failed to set SND socket buffer to desired size. Actual: %d, Desired: %d",
                         actualWriteSize,
                         writeBufferSize);
        }
    }
}


#if defined(YARP_HAS_ACE)
int DgramTwoWayStream::restrictMcast(ACE_SOCK_Dgram_Mcast* dmcast,
                                     const Contact& group,
                                     const Contact& ipLocal,
                                     bool add)
{
    restrictInterfaceIp = ipLocal;

    yCInfo(DGRAMTWOWAYSTREAM, "multicast connection %s on network interface for %s", group.getHost().c_str(), ipLocal.getHost().c_str());
    int result = -1;
    // There's some major damage in ACE mcast interfaces.
    // Most require interface names, yet provide no way to query
    // these names - and in the end, convert to IP addresses.
    // Here we try to do an end run around ACE.

    // based on: ACE_SOCK_Dgram::set_nic

    ip_mreq multicast_address;
    ACE_INET_Addr group_addr(group.getPort(),
                             group.getHost().c_str());
    ACE_INET_Addr interface_addr(ipLocal.getPort(),
                                 ipLocal.getHost().c_str());
    multicast_address.imr_interface.s_addr = htonl(interface_addr.get_ip_address());
    multicast_address.imr_multiaddr.s_addr = htonl(group_addr.get_ip_address());

    if (add) {
        yCDebug(DGRAMTWOWAYSTREAM, "Trying to correct mcast membership...");
        result = ((ACE_SOCK*)dmcast)->set_option(IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_address, sizeof(struct ip_mreq));
    } else {
        yCDebug(DGRAMTWOWAYSTREAM, "Trying to correct mcast output...");
        result = ((ACE_SOCK*)dmcast)->set_option(IPPROTO_IP, IP_MULTICAST_IF, &multicast_address.imr_interface.s_addr, sizeof(struct in_addr));
    }
    if (result != 0) {
        int num = errno;
        yCDebug(DGRAMTWOWAYSTREAM, "mcast result: %s", strerror(num));
        if (num == 98) {
            // our membership is already correct / Address already in use
            result = 0;
        }
        result = 0; // in fact, best to proceed for Windows.
    }

    return result;
}
#endif


bool DgramTwoWayStream::openMcast(const Contact& group,
                                  const Contact& ipLocal)
{

    multiMode = true;

    localAddress = ipLocal;

#if defined(YARP_HAS_ACE)
    localHandle = ACE_INET_Addr((u_short)(localAddress.getPort()),
                                (ACE_UINT32)INADDR_ANY);

    ACE_SOCK_Dgram_Mcast::options mcastOptions = ACE_SOCK_Dgram_Mcast::DEFOPTS;
#    if defined(__APPLE__)
    mcastOptions = static_cast<ACE_SOCK_Dgram_Mcast::options>(ACE_SOCK_Dgram_Mcast::OPT_BINDADDR_NO | ACE_SOCK_Dgram_Mcast::DEFOPT_NULLIFACE);
#    endif

    auto* dmcast = new ACE_SOCK_Dgram_Mcast(mcastOptions);
    dgram = dmcast;
    mgram = dmcast;
    yCAssert(DGRAMTWOWAYSTREAM, dgram != nullptr);

    int result = -1;
    ACE_INET_Addr addr(group.getPort(), group.getHost().c_str());
    result = dmcast->open(addr, nullptr, 1);
    if (result == 0) {
        result = restrictMcast(dmcast, group, ipLocal, false);
    }

    if (result != 0) {
        yCError(DGRAMTWOWAYSTREAM, "could not open multicast datagram socket");
        return false;
    }

#else
    dgram = nullptr;
    dgram_sockfd = -1;

    int s = -1;
    struct sockaddr_in dgram_sin;
    // create what looks like an ordinary UDP socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        yCError(DGRAMTWOWAYSTREAM, "could not create sender socket");
        std::exit(1);
    }
    // set up destination address
    memset((char*)&dgram_sin, 0, sizeof(dgram_sin));
    dgram_sin.sin_family = AF_INET;
    dgram_sin.sin_port = htons(group.getPort());


    if (inet_pton(AF_INET, group.getHost().c_str(), &dgram_sin.sin_addr) == 0) {
        yCError(DGRAMTWOWAYSTREAM, "could not set up mcast client");
        std::exit(1);
    }
    if (connect(s, (struct sockaddr*)&dgram_sin, sizeof(dgram_sin)) == -1) {
        yCError(DGRAMTWOWAYSTREAM, "could not connect mcast client");
        std::exit(1);
    }


    dgram_sockfd = s;
    dgram = this;
    int local_port = -1;

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(dgram_sockfd, (struct sockaddr*)&sin, &len) == 0 && sin.sin_family == AF_INET) {
        local_port = ntohs(sin.sin_port);
    }


#endif
    configureSystemBuffers();
    remoteAddress = group;
#ifdef YARP_HAS_ACE

    localHandle.set(localAddress.getPort(), localAddress.getHost().c_str());
    remoteHandle.set(remoteAddress.getPort(), remoteAddress.getHost().c_str());
#else

    remoteAddress = group;
    localAddress = Contact("127.0.0.1", local_port);
    localHandle = local_port;
    remoteHandle = remoteAddress.getPort();


#endif
    yCDebug(DGRAMTWOWAYSTREAM, "Update: DGRAM from %s to %s", localAddress.toURI().c_str(), remoteAddress.toURI().c_str());
    allocate();

    return true;
}


bool DgramTwoWayStream::join(const Contact& group, bool sender, const Contact& ipLocal)
{
    yCDebug(DGRAMTWOWAYSTREAM, "subscribing to mcast address %s for %s", group.toURI().c_str(), (sender ? "writing" : "reading"));

    multiMode = true;

    if (sender) {
        if (ipLocal.isValid()) {
            return openMcast(group, ipLocal);
        }
        // just use udp as normal
        return open(group);
    }

#if defined(YARP_HAS_ACE)
    ACE_SOCK_Dgram_Mcast::options mcastOptions = ACE_SOCK_Dgram_Mcast::DEFOPTS;
#    if defined(__APPLE__)
    mcastOptions = static_cast<ACE_SOCK_Dgram_Mcast::options>(ACE_SOCK_Dgram_Mcast::OPT_BINDADDR_NO | ACE_SOCK_Dgram_Mcast::DEFOPT_NULLIFACE);
#    endif

    auto* dmcast = new ACE_SOCK_Dgram_Mcast(mcastOptions);

    dgram = dmcast;
    mgram = dmcast;
    yCAssert(DGRAMTWOWAYSTREAM, dgram != nullptr);

    ACE_INET_Addr addr(group.getPort(), group.getHost().c_str());

    int result = -1;
    if (ipLocal.isValid()) {
        result = dmcast->join(addr, 1);

        if (result == 0) {
            result = restrictMcast(dmcast, group, ipLocal, true);
        }
    } else {
        result = dmcast->join(addr, 1);
    }

    if (result != 0) {
        yCError(DGRAMTWOWAYSTREAM, "cannot connect to multi-cast address");
        happy = false;
        return false;
    }
#else
    struct ip_mreq mreq;
    int s = -1;
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        yCError(DGRAMTWOWAYSTREAM, "could not create receiver socket");
        happy = false;
        return false;
    }
    struct sockaddr_in addr;
    u_int yes = 1;

    /* set up destination address */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(group.getPort());

    // allow multiple sockets to use the same PORT number
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(u_int)) < 0) {
        yCError(DGRAMTWOWAYSTREAM, "could not allow sockets use the same ADDRESS");
        happy = false;
        return false;
    }

#    if defined(__APPLE__)
    if (setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(u_int)) < 0) {
        yCError(DGRAMTWOWAYSTREAM, "could not allow sockets use the same PORT number");
        happy = false;
        return false;
    }
#    endif

    // bind to receive address
    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        yCError(DGRAMTWOWAYSTREAM, "could not create mcast server");
        happy = false;
        return false;
    }

    // use setsockopt() to request that the kernel join a multicast group
    if (inet_pton(AF_INET, group.getHost().c_str(), &mreq.imr_multiaddr) == 0) {
        yCError(DGRAMTWOWAYSTREAM, "Could not set up the mcast server");
        std::exit(1);
    }
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        yCError(DGRAMTWOWAYSTREAM, "could not join the multicast group");
        yCError(DGRAMTWOWAYSTREAM, "sendto: %d, %s", errno, strerror(errno));
        happy = false;
        return false;
    }

    dgram_sockfd = s;
    dgram = this;
#endif
    configureSystemBuffers();

    localAddress = group;
    remoteAddress = group;
#ifdef YARP_HAS_ACE
    localHandle.set(localAddress.getPort(), localAddress.getHost().c_str());
    remoteHandle.set(remoteAddress.getPort(), remoteAddress.getHost().c_str());
#else
    localHandle = localAddress.getPort();
    remoteHandle = remoteAddress.getPort();
#endif
    allocate();
    return true;
}

DgramTwoWayStream::~DgramTwoWayStream()
{
    closeMain();
}

void DgramTwoWayStream::interrupt()
{
    bool act = false;
    mutex.lock();
    if ((!closed) && (!interrupting) && happy) {
        act = true;
        interrupting = true;
        closed = true;
    }
    mutex.unlock();

    if (act) {
        if (reader) {
            int ct = 3;
            while (happy && ct > 0) {
                ct--;
                DgramTwoWayStream tmp;
                if (mgram != nullptr) {
                    yCDebug(DGRAMTWOWAYSTREAM, "* mcast interrupt, interface %s", restrictInterfaceIp.toString().c_str());
                    tmp.join(localAddress, true, restrictInterfaceIp);
                } else {
                    yCDebug(DGRAMTWOWAYSTREAM, "* dgram interrupt");
                    tmp.open(Contact(localAddress.getHost(), 0),
                             localAddress);
                }
                yCDebug(DGRAMTWOWAYSTREAM, "* interrupt state %s %s %s",
                           (interrupting ? "true" : "false"),
                           (closed ? "true" : "false"),
                           (happy ? "true" : "false"));
                ManagedBytes empty(10);
                for (size_t i = 0; i < empty.length(); i++) {
                    empty.get()[i] = 0;
                }

                // don't want this message getting into a valid packet
                tmp.pct = -1;

                tmp.write(empty.bytes());
                tmp.flush();
                tmp.close();
                if (happy) {
                    yarp::os::SystemClock::delaySystem(0.25);
                }
            }
            yCDebug(DGRAMTWOWAYSTREAM, "dgram interrupt done");
        }
        mutex.lock();
        interrupting = false;
        mutex.unlock();
    } else {
        // wait for interruption to be done
        if (interrupting) {
            while (interrupting) {
                yCDebug(DGRAMTWOWAYSTREAM, "waiting for dgram interrupt to be finished...");
                yarp::os::SystemClock::delaySystem(0.1);
            }
        }
    }
}

void DgramTwoWayStream::closeMain()
{
    if (dgram != nullptr) {
        //printf("Dgram closing, interrupt state %d\n", interrupting);
        interrupt();
        mutex.lock();
        closed = true;
        happy = false;
        //printf("Dgram closinger, interrupt state %d\n", interrupting);
        mutex.unlock();
        while (interrupting) {
            happy = false;
            yarp::os::SystemClock::delaySystem(0.1);
        }
        mutex.lock();
        if (dgram != nullptr) {
#if defined(YARP_HAS_ACE)
            dgram->close();
            delete dgram;
#else
            if (dgram_sockfd >= 0) {
                ::close(dgram_sockfd);
            }
            dgram_sockfd = -1;
#endif
            dgram = nullptr;
            mgram = nullptr;
        }
        happy = false;
        mutex.unlock();
    }
    happy = false;
}

yarp::conf::ssize_t DgramTwoWayStream::read(Bytes& b)
{
    reader = true;
    bool done = false;

    while (!done) {

        if (closed) {
            happy = false;
            return -1;
        }

        // if nothing is available, try to grab stuff
        if (readAvail == 0) {
            readAt = 0;


            //yCAssert(DGRAMTWOWAYSTREAM, dgram != nullptr);
            yCTrace(DGRAMTWOWAYSTREAM, "DGRAM Waiting for something!");
            yarp::conf::ssize_t result = -1;
#if defined(YARP_HAS_ACE)
            if ((dgram != nullptr) && restrictInterfaceIp.isValid()) {
                yCTrace(DGRAMTWOWAYSTREAM, "Consider remote mcast");
                yCTrace(DGRAMTWOWAYSTREAM, "What we know:");
                yCTrace(DGRAMTWOWAYSTREAM, "  %s", restrictInterfaceIp.toString().c_str());
                yCTrace(DGRAMTWOWAYSTREAM, "  %s", localAddress.toString().c_str());
                yCTrace(DGRAMTWOWAYSTREAM, "  %s", remoteAddress.toString().c_str());

                ACE_INET_Addr iface(restrictInterfaceIp.getPort(),
                                    restrictInterfaceIp.getHost().c_str());
                ACE_INET_Addr dummy((u_short)0, (ACE_UINT32)INADDR_ANY);
                result = dgram->recv(readBuffer.get(), readBuffer.length(), dummy);
                yCDebug(DGRAMTWOWAYSTREAM, "MCAST Got %zd bytes", result);

            } else
#endif
                if (dgram != nullptr) {
                yCAssert(DGRAMTWOWAYSTREAM, dgram != nullptr);
#if defined(YARP_HAS_ACE)
                ACE_INET_Addr dummy((u_short)0, (ACE_UINT32)INADDR_ANY);
                yCTrace(DGRAMTWOWAYSTREAM, "DGRAM Waiting for something!");
                result = dgram->recv(readBuffer.get(), readBuffer.length(), dummy);
#else
                result = recv(dgram_sockfd, readBuffer.get(), readBuffer.length(), 0);
#endif
                yCDebug(DGRAMTWOWAYSTREAM, "DGRAM Got %zd bytes", result);
            } else {
                onMonitorInput();
                //printf("Monitored input of %d bytes\n", monitor.length());
                if (monitor.length() > readBuffer.length()) {
                    printf("Too big!\n");
                    std::exit(1);
                }
                memcpy(readBuffer.get(), monitor.get(), monitor.length());
                result = monitor.length();
            }

            if (closed || (result < 0)) {
                happy = false;
                return -1;
            }
            readAvail = result;

            // deal with CRC
            int altPct = 0;
            bool crcOk = checkCrc(readBuffer.get(), readAvail, CRC_SIZE, pct, &altPct);
            if (altPct != -1) {
                pct++;
                if (!crcOk) {
                    if (bufferAlertNeeded && !bufferAlerted) {
                        yCError(DGRAMTWOWAYSTREAM, "*** Multicast/UDP packet dropped - checksum error ***");
                        yCInfo(DGRAMTWOWAYSTREAM, "The UDP/MCAST system buffer limit on your system is low.");
                        yCInfo(DGRAMTWOWAYSTREAM, "You may get packet loss under heavy conditions.");
#ifdef __linux__
                        yCInfo(DGRAMTWOWAYSTREAM, "To change the buffer limit on linux: sysctl -w net.core.rmem_max=8388608");
                        yCInfo(DGRAMTWOWAYSTREAM, "(Might be something like: sudo /sbin/sysctl -w net.core.rmem_max=8388608)");
#else
                        yCInfo(DGRAMTWOWAYSTREAM, "To change the limit use: sysctl for Linux/FreeBSD, ndd for Solaris, no for AIX");
#endif
                        bufferAlerted = true;
                    } else {
                        errCount++;
                        double now = SystemClock::nowSystem();
                        if (now - lastReportTime > 1) {
                            yCError(DGRAMTWOWAYSTREAM, "*** %d datagram packet(s) dropped - checksum error ***", errCount);
                            lastReportTime = now;
                            errCount = 0;
                        }
                    }
                    reset();
                    return -1;
                }
                readAt += CRC_SIZE;
                readAvail -= CRC_SIZE;
                done = true;
            } else {
                readAvail = 0;
            }
        }

        // if stuff is available, take it
        if (readAvail > 0) {
            size_t take = readAvail;
            if (take > b.length()) {
                take = b.length();
            }
            memcpy(b.get(), readBuffer.get() + readAt, take);
            readAt += take;
            readAvail -= take;
            return take;
        }
    }

    return 0;
}

void DgramTwoWayStream::write(const Bytes& b)
{
    yCTrace(DGRAMTWOWAYSTREAM, "DGRAM prep writing");
    yCTrace(DGRAMTWOWAYSTREAM, "DGRAM write %zu bytes", b.length());

    if (reader) {
        return;
    }
    if (writeBuffer.get() == nullptr) {
        return;
    }

    Bytes local = b;
    while (local.length() > 0) {
        yCTrace(DGRAMTWOWAYSTREAM, "DGRAM prep writing");
        yarp::conf::ssize_t rem = local.length();
        yarp::conf::ssize_t space = writeBuffer.length() - writeAvail;
        bool shouldFlush = false;
        if (rem >= space) {
            rem = space;
            shouldFlush = true;
        }
        memcpy(writeBuffer.get() + writeAvail, local.get(), rem);
        writeAvail += rem;
        local = Bytes(local.get() + rem, local.length() - rem);
        if (shouldFlush) {
            flush();
        }
    }
}


void DgramTwoWayStream::flush()
{
    if (writeBuffer.get() == nullptr) {
        return;
    }

    // should set CRC
    if (writeAvail <= CRC_SIZE) {
        return;
    }
    addCrc(writeBuffer.get(), writeAvail, CRC_SIZE, pct);
    pct++;

    if (writeAvail > 0) {
        //yCAssert(DGRAMTWOWAYSTREAM, dgram != nullptr);
        yarp::conf::ssize_t len = 0;

#if defined(YARP_HAS_ACE)
        if (mgram != nullptr) {
            len = mgram->send(writeBuffer.get(), writeAvail);
            yCDebug(DGRAMTWOWAYSTREAM, "MCAST - wrote %zd bytes", len);
        } else
#endif
            if (dgram != nullptr) {
#if defined(YARP_HAS_ACE)
            len = dgram->send(writeBuffer.get(), writeAvail, remoteHandle);
#else
            len = send(dgram_sockfd, writeBuffer.get(), writeAvail, 0);
#endif
            yCDebug(DGRAMTWOWAYSTREAM, "DGRAM - wrote %zd bytes to %s", len, remoteAddress.toString().c_str());
        } else {
            Bytes b(writeBuffer.get(), writeAvail);
            monitor = ManagedBytes(b, false);
            monitor.copy();
            //printf("Monitored output of %d bytes\n", monitor.length());
            len = monitor.length();
            onMonitorOutput();
        }
        if (len > writeBuffer.length() * 0.75) {
            yCDebug(DGRAMTWOWAYSTREAM, "long dgrams might need a little time");

            // Under heavy loads, packets could get dropped
            // 640x480x3 images correspond to about 15 datagrams
            // so there's not much time possible between them
            // looked at iperf, it just does a busy-waiting delay
            // there's an implementation below, but commented out -
            // better solution was to increase recv buffer size

            double first = yarp::os::SystemClock::nowSystem();
            double now;
            int ct = 0;
            do {
                //printf("Busy wait... %d\n", ct);
                yarp::os::SystemClock::delaySystem(0);
                now = yarp::os::SystemClock::nowSystem();
                ct++;
            } while (now - first < 0.001);
        }

        if (len < 0) {
            happy = false;
            yCDebug(DGRAMTWOWAYSTREAM, "DGRAM failed to send message with error: %s", strerror(errno));
            return;
        }
        writeAvail -= len;

        if (writeAvail != 0) {
            // well, we have a problem
            // checksums will cause dumping
            yCDebug(DGRAMTWOWAYSTREAM, "dgram/mcast send behaving badly");
        }
    }
    // finally: writeAvail should be 0

    // make space for CRC
    writeAvail = CRC_SIZE;
}


bool DgramTwoWayStream::isOk() const
{
    return happy;
}


void DgramTwoWayStream::reset()
{
    readAt = 0;
    readAvail = 0;
    writeAvail = CRC_SIZE;
    pct = 0;
}


void DgramTwoWayStream::beginPacket()
{
//     yCError(DGRAMTWOWAYSTREAM, "Packet begins: %s", (reader ? "reader" : "writer"));
    pct = 0;
}

void DgramTwoWayStream::endPacket()
{
//     yCError(DGRAMTWOWAYSTREAM, "Packet ends: %s", (reader ? "reader" : "writer"));
    if (!reader) {
        pct = 0;
    }
}

Bytes DgramTwoWayStream::getMonitor()
{
    return monitor.bytes();
}


void DgramTwoWayStream::removeMonitor()
{
    monitor.clear();
}


bool DgramTwoWayStream::setTypeOfService(int tos)
{
    if (dgram == nullptr) {
        return false;
    }
#if defined(YARP_HAS_ACE)
    return (dgram->set_option(IPPROTO_IP, IP_TOS, (int*)&tos, (int)sizeof(tos)) == 0);
#else
    return (setsockopt(dgram_sockfd, IPPROTO_IP, IP_TOS, (int*)&tos, (int)sizeof(tos)) == 0);
#endif
}

int DgramTwoWayStream::getTypeOfService()
{
    int tos = -1;
    if (dgram == nullptr) {
        return tos;
    }
#if defined(YARP_HAS_ACE)
    int optlen;
    dgram->get_option(IPPROTO_IP, IP_TOS, (int*)&tos, &optlen);
#else
    socklen_t optlen;
    getsockopt(dgram_sockfd, IPPROTO_IP, IP_TOS, (int*)&tos, &optlen);
#endif
    return tos;
}

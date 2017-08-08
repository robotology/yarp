/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/system.h>

#include <yarp/os/impl/DgramTwoWayStream.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/Time.h>
#include <yarp/os/NetType.h>

#if defined(YARP_HAS_ACE)
#  include <ace/SOCK_Dgram_Mcast.h>
#  include <ace/SOCK_Dgram.h>
#  include <ace/Handle_Set.h>
#  include <ace/Log_Msg.h>
#  include <ace/INET_Addr.h>
#  include <ace/ACE.h>
#  include <ace/OS_Memory.h>
#  include <ace/OS_NS_sys_select.h>
#  include <ace/os_include/net/os_if.h>
#else
#  include <arpa/inet.h>
#  include <netinet/in.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <unistd.h>
#endif

#include <cerrno>
#include <cstring>

using namespace yarp::os::impl;
using namespace yarp::os;

#define CRC_SIZE 8
#define READ_SIZE (120000-CRC_SIZE)
#define WRITE_SIZE (60000-CRC_SIZE)


static bool checkCrc(char *buf, YARP_SSIZE_T length, YARP_SSIZE_T crcLength, int pct,
                     int *store_altPct = YARP_NULLPTR) {
    NetInt32 alt =
        (NetInt32)NetType::getCrc(buf+crcLength, (length>crcLength)?(length-crcLength):0);
    Bytes b(buf, 4);
    Bytes b2(buf+4, 4);
    NetInt32 curr = NetType::netInt(b);
    int altPct = NetType::netInt(b2);
    bool ok = (alt == curr && pct==altPct);
    if (!ok) {
        if (alt!=curr) {
            YARP_DEBUG(Logger::get(), "crc mismatch");
        }
        if (pct!=altPct) {
            YARP_DEBUG(Logger::get(), "packet code broken");
        }
    }
    if (store_altPct != YARP_NULLPTR) {
        *store_altPct = altPct;
    }

    return ok;
}


static void addCrc(char *buf, YARP_SSIZE_T length, YARP_SSIZE_T crcLength, int pct) {
    NetInt32 alt =
        (NetInt32)NetType::getCrc(buf+crcLength,
                                           (length>crcLength)?(length-crcLength):0);
    Bytes b(buf, 4);
    Bytes b2(buf+4, 4);
    NetType::netInt((NetInt32)alt, b);
    NetType::netInt((NetInt32)pct, b2);
}


bool DgramTwoWayStream::open(const Contact& remote) {
#if defined(YARP_HAS_ACE)
    ACE_INET_Addr anywhere((u_short)0, (ACE_UINT32)INADDR_ANY);
    Contact local(anywhere.get_host_addr(),
                  anywhere.get_port_number());
#else
    Contact local("localhost", -1);
#endif
    return open(local, remote);
}

bool DgramTwoWayStream::open(const Contact& local, const Contact& remote) {
    localAddress = local;
    remoteAddress = remote;

#if defined(YARP_HAS_ACE)
    localHandle = ACE_INET_Addr((u_short)(localAddress.getPort()), (ACE_UINT32)INADDR_ANY);
    if (remote.isValid()) {
        remoteHandle.set(remoteAddress.getPort(), remoteAddress.getHost().c_str());
    }
    dgram = new ACE_SOCK_Dgram;
    yAssert(dgram != YARP_NULLPTR);

    int result = dgram->open(localHandle,
                             ACE_PROTOCOL_FAMILY_INET,
                             0,
                             1);
#else
    dgram = YARP_NULLPTR;
    dgram_sockfd = -1;

    int s = -1;
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
        std::exit(1);
    }
    struct sockaddr_in dgram_sin;
    memset((char *) &dgram_sin, 0, sizeof(dgram_sin));
    dgram_sin.sin_family = AF_INET;
    dgram_sin.sin_addr.s_addr = htonl(INADDR_ANY);
    dgram_sin.sin_port = htons(remote.getPort());
    if (local.isValid()) {
        if (inet_pton(AF_INET, remote.getHost().c_str(), &dgram_sin.sin_addr)==0) {
            YARP_ERROR(Logger::get(), "could not set up udp client\n");
            std::exit(1);
        }
        if (connect(s, (struct sockaddr *)&dgram_sin, sizeof(dgram_sin))==-1) {
            YARP_ERROR(Logger::get(), "could not connect udp client\n");
            std::exit(1);
        }
    } else {
        if (bind(s, (struct sockaddr *)&dgram_sin, sizeof(dgram_sin))==-1) {
            YARP_ERROR(Logger::get(), "could not create udp server\n");
            std::exit(1);
        }
    }
    dgram_sockfd = s;
    dgram = this;
    int result = -1;
    int local_port = -1;

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(dgram_sockfd, (struct sockaddr *)&sin, &len) == 0 &&
        sin.sin_family == AF_INET) {
        result = 0;
        local_port = ntohs(sin.sin_port);
    }
#endif

    if (result!=0) {
        YARP_ERROR(Logger::get(), "could not open datagram socket");
        return false;
    }

    configureSystemBuffers();

#if defined(YARP_HAS_ACE)
    dgram->get_local_addr(localHandle);
    YARP_DEBUG(Logger::get(), ConstString("starting DGRAM entity on port number ") + NetType::toString(localHandle.get_port_number()));
    localAddress = Contact("127.0.0.1",
                           localHandle.get_port_number());
#else
    localAddress = Contact("127.0.0.1", local_port);
#endif

    YARP_DEBUG(Logger::get(), ConstString("Update: DGRAM from ") +
               localAddress.toURI() +
               " to " + remoteAddress.toURI());

    allocate();

    return true;
}

void DgramTwoWayStream::allocate(int readSize, int writeSize) {
#if defined(__APPLE__)
    //These are only as another default. We should modify the method to return bool
    //and fail if we cannot read the socket size.

    int _read_size = READ_SIZE+CRC_SIZE;
    int _write_size = WRITE_SIZE+CRC_SIZE;

    int socketSendBufferSize = -1;
    int socketRecvBufferSize = -1;

#if defined(YARP_HAS_ACE)
    //Defaults to socket size
    if (dgram) {
        int len = sizeof(_read_size);
        int result = dgram->get_option(SOL_SOCKET, SO_SNDBUF, &_write_size, &len);
        if (result < 0) {
            YARP_ERROR(Logger::get(), ConstString("Failed to read buffer size from SNDBUF socket with error: ") +
                       ConstString(strerror(errno)));
        }
        socketSendBufferSize = _write_size;

        result = dgram->get_option(SOL_SOCKET, SO_RCVBUF, &_read_size, &len);
        if (result < 0) {
            YARP_ERROR(Logger::get(), ConstString("Failed to read buffer size from RCVBUF socket with error: ") +
                       ConstString(strerror(errno)));
        }
        socketRecvBufferSize = _read_size;
    }
#else
    socklen_t len = sizeof(_read_size);
    int result = getsockopt(dgram_sockfd, SOL_SOCKET, SO_SNDBUF, &_write_size, &len);
    if (result < 0) {
        YARP_ERROR(Logger::get(), ConstString("Failed to read buffer size from SNDBUF socket with error: ") +
                   ConstString(strerror(errno)));
    }
    socketSendBufferSize = _write_size;

    result = getsockopt(dgram_sockfd, SOL_SOCKET, SO_RCVBUF, &_read_size, &len);
    if (result < 0) {
        YARP_ERROR(Logger::get(), ConstString("Failed to read buffer size from RCVBUF socket with error: ") +
                   ConstString(strerror(errno)));
    }
    socketRecvBufferSize = _read_size;
#endif

    ConstString _env_dgram = NetworkBase::getEnvironment("YARP_DGRAM_SIZE");
    ConstString _env_mode = "";
    if (multiMode) {
        _env_mode = NetworkBase::getEnvironment("YARP_MCAST_SIZE");
    } else {
        _env_mode = NetworkBase::getEnvironment("YARP_UDP_SIZE");
    }
    if ( _env_mode!="") {
        _env_dgram = _env_mode;
    }
    if (_env_dgram!="") {
        int sz = NetType::toInt(_env_dgram);
        if (sz!=0) {
            _read_size = _write_size = sz;
        }
        YARP_INFO(Logger::get(), ConstString("Datagram packet size set to ") +
                  NetType::toString(_read_size));
    }
    if (readSize!=0) {
        _read_size = readSize;
        YARP_INFO(Logger::get(), ConstString("Datagram read size reset to ") +
                  NetType::toString(_read_size));
    }
    if (writeSize!=0) {
        _write_size = writeSize;
        YARP_INFO(Logger::get(), ConstString("Datagram write size reset to ") +
                  NetType::toString(_write_size));
    }
    readBuffer.allocate(_read_size);
    writeBuffer.allocate(_write_size);
    readAt = 0;
    readAvail = 0;
    writeAvail = CRC_SIZE;
    //happy = true;
    pct = 0;

    if (_read_size < socketRecvBufferSize && socketRecvBufferSize != -1) {
        YARP_WARN(Logger::get(), "RECV buffer size smaller than socket RECV size. Errors can occur during reading");
    }
    if (_write_size > socketSendBufferSize && socketSendBufferSize != -1) {
        YARP_WARN(Logger::get(), "SND buffer size bigger than socket SND size. Errors can occur during writing");
    }

#else
    int _read_size = READ_SIZE+CRC_SIZE;
    int _write_size = WRITE_SIZE+CRC_SIZE;

    ConstString _env_dgram = NetworkBase::getEnvironment("YARP_DGRAM_SIZE");
    ConstString _env_mode = "";
    if (multiMode) {
        _env_mode = NetworkBase::getEnvironment("YARP_MCAST_SIZE");
    } else {
        _env_mode = NetworkBase::getEnvironment("YARP_UDP_SIZE");
    }
    if ( _env_mode!="") {
        _env_dgram = _env_mode;
    }
    if (_env_dgram!="") {
        int sz = NetType::toInt(_env_dgram);
        if (sz!=0) {
            _read_size = _write_size = sz;
        }
        YARP_INFO(Logger::get(), ConstString("Datagram packet size set to ") +
                  NetType::toString(_read_size));
    }
    if (readSize!=0) {
        _read_size = readSize;
        YARP_INFO(Logger::get(), ConstString("Datagram read size reset to ") +
                  NetType::toString(_read_size));
    }
    if (writeSize!=0) {
        _write_size = writeSize;
        YARP_INFO(Logger::get(), ConstString("Datagram write size reset to ") +
                  NetType::toString(_write_size));
    }
    readBuffer.allocate(_read_size);
    writeBuffer.allocate(_write_size);
    readAt = 0;
    readAvail = 0;
    writeAvail = CRC_SIZE;
    //happy = true;
    pct = 0;
#endif
}


void DgramTwoWayStream::configureSystemBuffers() {
#if defined(__APPLE__)
    //By default use system buffer size.
    //These can be overwritten by environment variables
    //Generic variable
    ConstString socketBufferSize = NetworkBase::getEnvironment("YARP_DGRAM_BUFFER_SIZE");
    //Specific read
    ConstString socketReadBufferSize = NetworkBase::getEnvironment("YARP_DGRAM_RECV_BUFFER_SIZE");
    //Specific write
    ConstString socketSendBufferSize = NetworkBase::getEnvironment("YARP_DGRAM_SND_BUFFER_SIZE");

    int readBufferSize = -1;
    if (socketReadBufferSize != "") {
        readBufferSize = NetType::toInt(socketReadBufferSize);
    } else if (socketBufferSize != "") {
        readBufferSize = NetType::toInt(socketBufferSize);
    }

    int writeBufferSize = -1;
    if (socketSendBufferSize != "") {
        writeBufferSize = NetType::toInt(socketSendBufferSize);
    } else if (socketBufferSize != "") {
        writeBufferSize = NetType::toInt(socketBufferSize);
    }

    if (readBufferSize > 0) {
        int actualReadSize = -1;

#if defined(YARP_HAS_ACE)
        int intSize = sizeof(readBufferSize);
        int setResult = dgram->set_option(SOL_SOCKET, SO_RCVBUF,
                                          (void*)&readBufferSize, intSize);

        int getResult = dgram->get_option(SOL_SOCKET, SO_RCVBUF,
                                          (void*)&actualReadSize, &intSize);
#else
        socklen_t intSize = sizeof(readBufferSize);
        int setResult = setsockopt(dgram_sockfd, SOL_SOCKET, SO_RCVBUF,
                                   (void*)&readBufferSize, intSize);
        int getResult = getsockopt(dgram_sockfd, SOL_SOCKET, SO_RCVBUF,
                                   (void *) &actualReadSize, &intSize);
#endif
        if (setResult < 0 || getResult < 0 || readBufferSize != actualReadSize) {
            bufferAlertNeeded = true;
            bufferAlerted = false;
            YARP_WARN(Logger::get(), "Failed to set RECV socket buffer to desired size. Actual: " + NetType::toString(actualReadSize) + ", Desired: " + NetType::toString(readBufferSize));
        }
    }
    if (writeBufferSize > 0) {
        int actualWriteSize = -1;
#if defined(YARP_HAS_ACE)
        int intSize = sizeof(writeBufferSize);
        int setResult = dgram->set_option(SOL_SOCKET, SO_SNDBUF,
                                          (void*)&writeBufferSize, intSize);
        int getResult = dgram->get_option(SOL_SOCKET, SO_SNDBUF,
                                          (void*)&actualWriteSize, &intSize);
#else
        socklen_t intSize = sizeof(readBufferSize);
        int setResult = setsockopt(dgram_sockfd, SOL_SOCKET, SO_SNDBUF,
                                   (void*)&writeBufferSize, intSize);
        int getResult = getsockopt(dgram_sockfd, SOL_SOCKET, SO_SNDBUF,
                                   (void *) &actualWriteSize, &intSize);
#endif
        if (setResult < 0 || getResult < 0 || writeBufferSize != actualWriteSize) {
            bufferAlertNeeded = true;
            bufferAlerted = false;
            YARP_WARN(Logger::get(), "Failed to set SND socket buffer to desired size. Actual: " + NetType::toString(actualWriteSize) + ", Desired: " + NetType::toString(writeBufferSize));
        }
    }



#else
    // ask for more buffer space for udp/mcast

    ConstString _dgram_buffer_size = NetworkBase::getEnvironment("YARP_DGRAM_BUFFER_SIZE");

     int window_size_desired = 600000;

    if (_dgram_buffer_size!="")
        window_size_desired = NetType::toInt(_dgram_buffer_size);

    int window_size = window_size_desired;
#if defined(YARP_HAS_ACE)
    int result = dgram->set_option(SOL_SOCKET, SO_RCVBUF,
                                   (char *) &window_size, sizeof(window_size));
    dgram->set_option(SOL_SOCKET, SO_SNDBUF,
                                   (char *) &window_size, sizeof(window_size));
#else
    int result = setsockopt(dgram_sockfd, SOL_SOCKET, SO_RCVBUF,
                            (void*) &window_size, sizeof(window_size));
#endif

    window_size = 0;
#if defined(YARP_HAS_ACE)
    int len = 4;
    int result2 = dgram->get_option(SOL_SOCKET, SO_RCVBUF,
                                    (char *) &window_size, &len);
#else
    socklen_t len = 4;
    int result2 = getsockopt(dgram_sockfd, SOL_SOCKET, SO_RCVBUF,
                             (void *) &window_size, &len);
#endif
    if (result!=0||result2!=0||window_size<window_size_desired) {
        // give a warning if we get CRC problems
        bufferAlertNeeded = true;
        bufferAlerted = false;
    }
    YARP_DEBUG(Logger::get(),
               ConstString("Warning: buffer size set to ")+ NetType::toString(window_size) + ConstString(", you requested ") + NetType::toString(window_size_desired));
#endif
}


#if defined(YARP_HAS_ACE)
int DgramTwoWayStream::restrictMcast(ACE_SOCK_Dgram_Mcast * dmcast,
                                     const Contact& group,
                                     const Contact& ipLocal,
                                     bool add) {
    restrictInterfaceIp = ipLocal;

    YARP_INFO(Logger::get(),
              ConstString("multicast connection ") + group.getHost() + " on network interface for " + ipLocal.getHost());
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
    multicast_address.imr_interface.s_addr =
        htonl (interface_addr.get_ip_address ());
    multicast_address.imr_multiaddr.s_addr =
        htonl (group_addr.get_ip_address ());

    if (add) {
        YARP_DEBUG(Logger::get(), "Trying to correct mcast membership...\n");
        result =
            ((ACE_SOCK*)dmcast)->set_option (IPPROTO_IP,
                                             IP_ADD_MEMBERSHIP,
                                             &multicast_address,
                                             sizeof (struct ip_mreq));
    } else {
        YARP_DEBUG(Logger::get(), "Trying to correct mcast output...");
        result =
            ((ACE_SOCK*)dmcast)->set_option (IPPROTO_IP,
                                             IP_MULTICAST_IF,
                                             &multicast_address.imr_interface.s_addr,
                                             sizeof (struct in_addr));

    }
    if (result!=0) {
        int num = errno;
        YARP_DEBUG(Logger::get(),
                   ConstString("mcast result: ") +
                   strerror(num));
        if (num==98) {
            // our membership is already correct / Address already in use
            result = 0;
        }
        result = 0; // in fact, best to proceed for Windows.
    }

    return result;
}
#endif


bool DgramTwoWayStream::openMcast(const Contact& group,
                                  const Contact& ipLocal) {

    multiMode = true;

    localAddress = ipLocal;

#if defined(YARP_HAS_ACE)
    localHandle = ACE_INET_Addr((u_short)(localAddress.getPort()),
                                (ACE_UINT32)INADDR_ANY);

    ACE_SOCK_Dgram_Mcast::options mcastOptions = ACE_SOCK_Dgram_Mcast::DEFOPTS;
    #if defined(__APPLE__)
    mcastOptions = static_cast<ACE_SOCK_Dgram_Mcast::options>(ACE_SOCK_Dgram_Mcast::OPT_BINDADDR_NO | ACE_SOCK_Dgram_Mcast::DEFOPT_NULLIFACE);
    #endif

    ACE_SOCK_Dgram_Mcast *dmcast = new ACE_SOCK_Dgram_Mcast(mcastOptions);
    dgram = dmcast;
    mgram = dmcast;
    yAssert(dgram != YARP_NULLPTR);

    int result = -1;
    ACE_INET_Addr addr(group.getPort(), group.getHost().c_str());
    result = dmcast->open(addr, YARP_NULLPTR, 1);
    if (result==0) {
        result = restrictMcast(dmcast, group, ipLocal, false);
    }

    if (result!=0) {
        YARP_ERROR(Logger::get(), "could not open multicast datagram socket");
        return false;
    }

#else
    dgram = YARP_NULLPTR;
    dgram_sockfd = -1;

    int s = -1;
    struct sockaddr_in dgram_sin;
    // create what looks like an ordinary UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
        YARP_ERROR(Logger::get(), "could not create sender socket\n");
        std::exit(1);
    }
    // set up destination address
    memset((char *) &dgram_sin, 0, sizeof(dgram_sin));
    dgram_sin.sin_family = AF_INET;
    dgram_sin.sin_port = htons(group.getPort());


    if (inet_pton(AF_INET, group.getHost().c_str(), &dgram_sin.sin_addr)==0) {
        YARP_ERROR(Logger::get(), "could not set up mcast client\n");
        std::exit(1);
    }
    if (connect(s, (struct sockaddr *)&dgram_sin, sizeof(dgram_sin))==-1) {
        YARP_ERROR(Logger::get(), "could not connect mcast client\n");
        std::exit(1);
    }


    dgram_sockfd = s;
    dgram = this;
    int local_port = -1;

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(dgram_sockfd, (struct sockaddr *)&sin, &len) == 0 &&
        sin.sin_family == AF_INET) {
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
    YARP_DEBUG(Logger::get(), ConstString("Update: DGRAM from ") +
               localAddress.toURI() +
               " to " + remoteAddress.toURI());
    allocate();

    return true;
}


bool DgramTwoWayStream::join(const Contact& group, bool sender,
                             const Contact& ipLocal) {

    YARP_DEBUG(Logger::get(), ConstString("subscribing to mcast address ") +
               group.toURI() + " for " +
               (sender?"writing":"reading"));

    multiMode = true;

    if (sender) {
        if (ipLocal.isValid()) {
            return openMcast(group, ipLocal);
        } else {
            // just use udp as normal
            return open(group);
        }
    }

#if defined(YARP_HAS_ACE)
    ACE_SOCK_Dgram_Mcast::options mcastOptions = ACE_SOCK_Dgram_Mcast::DEFOPTS;
#if defined(__APPLE__)
    mcastOptions = static_cast<ACE_SOCK_Dgram_Mcast::options>(ACE_SOCK_Dgram_Mcast::OPT_BINDADDR_NO | ACE_SOCK_Dgram_Mcast::DEFOPT_NULLIFACE);
#endif

    ACE_SOCK_Dgram_Mcast *dmcast = new ACE_SOCK_Dgram_Mcast(mcastOptions);

    dgram = dmcast;
    mgram = dmcast;
    yAssert(dgram != YARP_NULLPTR);

    ACE_INET_Addr addr(group.getPort(), group.getHost().c_str());

    int result = -1;
    if (ipLocal.isValid()) {
        result = 0;
        result = dmcast->join(addr, 1);

        if (result==0) {
            result = restrictMcast(dmcast, group, ipLocal, true);
        }
    } else {
        result = dmcast->join(addr, 1);
    }

    if (result!=0) {
        YARP_ERROR(Logger::get(), "cannot connect to multi-cast address");
        happy = false;
        return false;
    }
#else
    struct ip_mreq mreq;
    int s = -1;
    if ((s=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
        YARP_ERROR(Logger::get(), "could not create receiver socket\n");
        happy = false;
        return false;
    }
    struct sockaddr_in addr;
    u_int yes=1;

    /* set up destination address */
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(group.getPort());

    // allow multiple sockets to use the same PORT number
    if (setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(u_int)) < 0)
    {
        YARP_ERROR(Logger::get(), "could not allow sockets use the same ADDRESS\n");
        happy = false;
        return false;
    }

#if defined (__APPLE__)
    if (setsockopt(s,SOL_SOCKET,SO_REUSEPORT,&yes,sizeof(u_int)) < 0)
    {
        YARP_ERROR(Logger::get(), "could not allow sockets use the same PORT number\n");
        happy = false;
        return false;
    }
#endif

    // bind to receive address
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr))==-1)
    {
        YARP_ERROR(Logger::get(), "could not create mcast server\n");
        happy = false;
        return false;
    }

    // use setsockopt() to request that the kernel join a multicast group
    if(inet_pton(AF_INET, group.getHost().c_str(), &mreq.imr_multiaddr)==0)
    {
        YARP_ERROR(Logger::get(), "Could not set up the mcast server");
        std::exit(1);

    }
    mreq.imr_interface.s_addr=htonl(INADDR_ANY);
    if (setsockopt(s,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0)
    {
        YARP_ERROR(Logger::get(), "could not join the multicast group\n");
        perror("sendto");
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

DgramTwoWayStream::~DgramTwoWayStream() {
    closeMain();
}

void DgramTwoWayStream::interrupt() {
    bool act = false;
    mutex.wait();
    if ((!closed) && (!interrupting) && happy) {
        act = true;
        interrupting = true;
        closed = true;
    }
    mutex.post();

    if (act) {
        if (reader) {
            int ct = 3;
            while (happy && ct>0) {
                ct--;
                DgramTwoWayStream tmp;
                if (mgram) {
                    YARP_DEBUG(Logger::get(),
                               ConstString("* mcast interrupt, interface ") +
                               restrictInterfaceIp.toString().c_str()
                               );
                    tmp.join(localAddress, true,
                             restrictInterfaceIp);
                } else {
                    YARP_DEBUG(Logger::get(), "* dgram interrupt");
                    tmp.open(Contact(localAddress.getHost(), 0),
                             localAddress);
                }
                YARP_DEBUG(Logger::get(),
                           ConstString("* interrupt state ") +
                           NetType::toString(interrupting) + " " +
                           NetType::toString(closed) + " " +
                           NetType::toString(happy) + " ");
                ManagedBytes empty(10);
                for (size_t i=0; i<empty.length(); i++) {
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
            YARP_DEBUG(Logger::get(), "dgram interrupt done");
        }
        mutex.wait();
        interrupting = false;
        mutex.post();
    } else {
        // wait for interruption to be done
        if (interrupting) {
            while (interrupting) {
                YARP_DEBUG(Logger::get(),
                           "waiting for dgram interrupt to be finished...");
                yarp::os::SystemClock::delaySystem(0.1);
            }
        }
    }

}

void DgramTwoWayStream::closeMain() {
    if (dgram != YARP_NULLPTR) {
        //printf("Dgram closing, interrupt state %d\n", interrupting);
        interrupt();
        mutex.wait();
        closed = true;
        happy = false;
        //printf("Dgram closinger, interrupt state %d\n", interrupting);
        mutex.post();
        while (interrupting) {
            happy = false;
            yarp::os::SystemClock::delaySystem(0.1);
        }
        mutex.wait();
        if (dgram != YARP_NULLPTR) {
#if defined(YARP_HAS_ACE)
            dgram->close();
            delete dgram;
#else
            if (dgram_sockfd>=0) {
                ::close(dgram_sockfd);
            }
            dgram_sockfd = -1;
#endif
            dgram = YARP_NULLPTR;
            mgram = YARP_NULLPTR;
        }
        happy = false;
        mutex.post();
    }
    happy = false;
}

YARP_SSIZE_T DgramTwoWayStream::read(const Bytes& b) {
    reader = true;
    bool done = false;

    while (!done) {

        if (closed) {
            happy = false;
            return -1;
        }

        // if nothing is available, try to grab stuff
        if (readAvail==0) {
            readAt = 0;


            //yAssert(dgram != YARP_NULLPTR);
            //YARP_DEBUG(Logger::get(), "DGRAM Waiting for something!");
            YARP_SSIZE_T result = -1;
#if defined(YARP_HAS_ACE)
            if (mgram && restrictInterfaceIp.isValid()) {
                /*
                printf("Consider remote mcast\n");
                printf("What we know:\n");
                printf("  %s\n", restrictInterfaceIp.toString().c_str());
                printf("  %s\n", localAddress.toString().c_str());
                printf("  %s\n", remoteAddress.toString().c_str());
                */
                ACE_INET_Addr iface(restrictInterfaceIp.getPort(),
                                    restrictInterfaceIp.getHost().c_str());
                ACE_INET_Addr dummy((u_short)0, (ACE_UINT32)INADDR_ANY);
                result =
                    dgram->recv(readBuffer.get(), readBuffer.length(), dummy);
                YARP_DEBUG(Logger::get(),
                           ConstString("MCAST Got ") + NetType::toString((int)result) +
                           " bytes");

            } else
#endif
            if (dgram != YARP_NULLPTR) {
                yAssert(dgram != YARP_NULLPTR);
#if defined(YARP_HAS_ACE)
                ACE_INET_Addr dummy((u_short)0, (ACE_UINT32)INADDR_ANY);
                //YARP_DEBUG(Logger::get(), "DGRAM Waiting for something!");
                result =
                    dgram->recv(readBuffer.get(), readBuffer.length(), dummy);
#else
                result = recv(dgram_sockfd, readBuffer.get(), readBuffer.length(), 0);
#endif
                YARP_DEBUG(Logger::get(),
                           ConstString("DGRAM Got ") + NetType::toString((int)result) +
                           " bytes");
            } else {
                onMonitorInput();
                //printf("Monitored input of %d bytes\n", monitor.length());
                if (monitor.length()>readBuffer.length()) {
                    printf("Too big!\n");
                    std::exit(1);
                }
                memcpy(readBuffer.get(), monitor.get(), monitor.length());
                result = monitor.length();
            }


            /*
              // this message isn't needed anymore
            if (result>WRITE_SIZE*1.25) {
                YARP_ERROR(Logger::get(),
                           ConstString("Got big datagram: ")+NetType::toString(result)+
                           " bytes");
            }
            */
            if (closed||(result<0)) {
                happy = false;
                return -1;
            }
            readAvail = result;

            // deal with CRC
            int altPct = 0;
            bool crcOk = checkCrc(readBuffer.get(), readAvail, CRC_SIZE, pct,
                                  &altPct);
            if (altPct!=-1) {
                pct++;
                if (!crcOk) {
                    if (bufferAlertNeeded&&!bufferAlerted) {
                        YARP_ERROR(Logger::get(),
                                   "*** Multicast/UDP packet dropped - checksum error ***");
                        YARP_INFO(Logger::get(),
                                  "The UDP/MCAST system buffer limit on your system is low.");
                        YARP_INFO(Logger::get(),
                                  "You may get packet loss under heavy conditions.");
#ifdef __linux__
                        YARP_INFO(Logger::get(),
                                  "To change the buffer limit on linux: sysctl -w net.core.rmem_max=8388608");
                        YARP_INFO(Logger::get(),
                                  "(Might be something like: sudo /sbin/sysctl -w net.core.rmem_max=8388608)");
#else
                        YARP_INFO(Logger::get(),
                                  "To change the limit use: sysctl for Linux/FreeBSD, ndd for Solaris, no for AIX");
#endif
                        bufferAlerted = true;
                    } else {
                        errCount++;
                        double now = SystemClock::nowSystem();
                        if (now-lastReportTime>1) {
                            YARP_ERROR(Logger::get(),
                                       ConstString("*** ") + NetType::toString(errCount) + " datagram packet(s) dropped - checksum error ***");
                            lastReportTime = now;
                            errCount = 0;
                        }
                    }
                    reset();
                    return -1;
                } else {
                    readAt += CRC_SIZE;
                    readAvail -= CRC_SIZE;
                }
                done = true;
            } else {
                readAvail = 0;
            }
        }

        // if stuff is available, take it
        if (readAvail>0) {
            size_t take = readAvail;
            if (take>b.length()) {
                take = b.length();
            }
            memcpy(b.get(), readBuffer.get()+readAt, take);
            readAt += take;
            readAvail -= take;
            return take;
        }
    }

    return 0;
}

void DgramTwoWayStream::write(const Bytes& b) {
    //YARP_DEBUG(Logger::get(), "DGRAM prep writing");
    //printf("DGRAM write %d bytes\n", b.length());

    if (reader) {
        return;
    }
    if (writeBuffer.get() == YARP_NULLPTR) {
        return;
    }

    Bytes local = b;
    while (local.length()>0) {
        //YARP_DEBUG(Logger::get(), "DGRAM prep writing");
        YARP_SSIZE_T rem = local.length();
        YARP_SSIZE_T space = writeBuffer.length()-writeAvail;
        bool shouldFlush = false;
        if (rem>=space) {
            rem = space;
            shouldFlush = true;
        }
        memcpy(writeBuffer.get()+writeAvail, local.get(), rem);
        writeAvail+=rem;
        local = Bytes(local.get()+rem, local.length()-rem);
        if (shouldFlush) {
            flush();
        }
    }
}


void DgramTwoWayStream::flush() {
    if (writeBuffer.get() == YARP_NULLPTR) {
        return;
    }

    // should set CRC
    if (writeAvail<=CRC_SIZE) {
        return;
    }
    addCrc(writeBuffer.get(), writeAvail, CRC_SIZE, pct);
    pct++;

    while (writeAvail>0) {
        YARP_SSIZE_T writeAt = 0;
        //yAssert(dgram != YARP_NULLPTR);
        YARP_SSIZE_T len = 0;

#if defined(YARP_HAS_ACE)
        if (mgram != YARP_NULLPTR) {
            len = mgram->send(writeBuffer.get()+writeAt, writeAvail-writeAt);
            YARP_DEBUG(Logger::get(),
                       ConstString("MCAST - wrote ") +
                       NetType::toString((int)len) + " bytes"
                       );
        } else
#endif
            if (dgram != YARP_NULLPTR) {
#if defined(YARP_HAS_ACE)
            len = dgram->send(writeBuffer.get()+writeAt, writeAvail-writeAt,
                              remoteHandle);
#else
            len = send(dgram_sockfd, writeBuffer.get()+writeAt,
                       writeAvail-writeAt, 0);
#endif
            YARP_DEBUG(Logger::get(),
                       ConstString("DGRAM - wrote ") +
                       NetType::toString((int)len) + " bytes to " +
                       remoteAddress.toString()
                       );
        } else {
            Bytes b(writeBuffer.get()+writeAt, writeAvail-writeAt);
            monitor = ManagedBytes(b, false);
            monitor.copy();
            //printf("Monitored output of %d bytes\n", monitor.length());
            len = monitor.length();
            onMonitorOutput();
        }
        //if (len>WRITE_SIZE*0.75) {
        if (len>writeBuffer.length()*0.75) {
            YARP_DEBUG(Logger::get(),
                       "long dgrams might need a little time");

            // Under heavy loads, packets could get dropped
            // 640x480x3 images correspond to about 15 datagrams
            // so there's not much time possible between them
            // looked at iperf, it just does a busy-waiting delay
            // there's an implementation below, but commented out -
            // better solution was to increase recv buffer size

            double first = yarp::os::SystemClock::nowSystem();
            double now = first;
            int ct = 0;
            do {
                //printf("Busy wait... %d\n", ct);
                yarp::os::SystemClock::delaySystem(0);
                now = yarp::os::SystemClock::nowSystem();
                ct++;
            } while (now-first<0.001);
        }

        if (len < 0) {
            happy = false;
            YARP_DEBUG(Logger::get(), "DGRAM failed to send message with error: " + ConstString(strerror(errno)));
            return;
        }
        writeAt += len;
        writeAvail -= len;

        if (writeAvail!=0) {
            // well, we have a problem
            // checksums will cause dumping
            YARP_DEBUG(Logger::get(), "dgram/mcast send behaving badly");
        }
    }
    // finally: writeAvail should be 0

    // make space for CRC
    writeAvail = CRC_SIZE;
}


bool DgramTwoWayStream::isOk() {
    return happy;
}


void DgramTwoWayStream::reset() {
    readAt = 0;
    readAvail = 0;
    writeAvail = CRC_SIZE;
    pct = 0;
}


void DgramTwoWayStream::beginPacket() {
    //YARP_ERROR(Logger::get(), ConstString("Packet begins: ")+(reader?"reader":"writer"));
    pct = 0;
}

void DgramTwoWayStream::endPacket() {
    //YARP_ERROR(Logger::get(), ConstString("Packet ends: ")+(reader?"reader":"writer"));
    if (!reader) {
        pct = 0;
    }
}

Bytes DgramTwoWayStream::getMonitor() {
    return monitor.bytes();
}


void DgramTwoWayStream::removeMonitor() {
    monitor.clear();
}


bool DgramTwoWayStream::setTypeOfService(int tos) {
    if (!dgram)
        return false;
#if defined(YARP_HAS_ACE)
    return (dgram->set_option(IPPROTO_IP, IP_TOS,
                              (int *)&tos, (int)sizeof(tos) ) == 0);
#else
    return (setsockopt(dgram_sockfd, IPPROTO_IP, IP_TOS,
                       (int *)&tos, (int)sizeof(tos) ) == 0);
#endif

}

int DgramTwoWayStream::getTypeOfService() {
    int tos = -1;
    if (!dgram)
        return tos;
#if defined(YARP_HAS_ACE)
    int optlen;
    dgram->get_option(IPPROTO_IP, IP_TOS,
                      (int *)&tos, &optlen);
#else
    socklen_t optlen;
    getsockopt(dgram_sockfd, IPPROTO_IP, IP_TOS,
                (int *)&tos, &optlen);
#endif
    return tos;
}

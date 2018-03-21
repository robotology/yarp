/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Anne van Rossum <anne@almende.com>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_POSIX_TCPSTREAM_H
#define YARP_OS_IMPL_POSIX_TCPSTREAM_H

// General files
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>

//#include <yarp/os/impl/PlatformSignal.h>
//#include <yarp/os/impl/PlatformUnistd.h>
//#include <yarp/os/impl/PlatformSysWait.h>

namespace yarp {
namespace os {
namespace impl {
namespace posix {


/* **************************************************************************************
 * Interface of TcpStream
 * **************************************************************************************/


class TcpStream
{
public:
    /**
     * Constructor TcpStream
     */
    TcpStream();

    /**
     * Destructor ~TcpStream
     */
    virtual ~TcpStream();

    inline ssize_t recv_n (void *buf, size_t n)
    {
        return ::recv(sd, buf, n, 0);
    }

    inline ssize_t recv_n (void *buf, size_t n, struct timeval *tv)
    {
        setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char *)tv, sizeof (*tv));
        return ::recv(sd, buf, n, 0);
    }

    inline ssize_t recv (void *buf, size_t n)
    {
        return ::recv(sd, buf, n, 0);
    }

    inline ssize_t recv (void *buf, size_t n, struct timeval *tv)
    {
        setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char *)tv, sizeof (*tv));
        return ::recv(sd, buf, n, 0);
    }

    inline ssize_t send_n (const void *buf, size_t n)
    {
        return ::send(sd, buf, n, 0);
    }

    inline ssize_t send_n (const void *buf, size_t n, struct timeval *tv)
    {
        setsockopt(sd, SOL_SOCKET, SO_SNDTIMEO, (char *)tv, sizeof (*tv));
        return ::send(sd, buf, n, 0);
    }

    // No idea what this should do...
    void flush() { }

    void close_reader()
    {
        if (sd!=-1) {
            ::shutdown(sd, SHUT_RD);
        }
    }

    void close_writer()
    {
        if (sd!=-1) {
            ::shutdown(sd, SHUT_WR);
        }
    }

    void close()
    {
        if (sd!=-1) {
            ::close(sd);
            sd = -1;
        }
    }

    int open();

    int get_local_addr (sockaddr &);

    int get_remote_addr (sockaddr &);

    // get stream descriptor
    int get_handle() { return sd; }

    // set stream descriptor
    void set_handle(int h) { sd = h; }

    // Wrapper around the setsockopt system call.
    inline int set_option(int level, int option, void *optval, int optlen) const
    {
        return setsockopt(sd, level, option, (char*)optval, optlen);
    }

    // Wrapper around the getsockopt system call.
    inline int get_option(int level, int option, void *optval, int *optlen) const
    {
        return getsockopt(sd, level, option, (char*)optval, (socklen_t*)optlen);
    }
private:
    // stream descriptor
    int sd;
};

} // namespace posix
} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_POSIX_TCPSTREAM_H

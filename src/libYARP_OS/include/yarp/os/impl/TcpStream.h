/**
 * @file TcpStream.h
 * @brief
 *
 * This file is created at Almende B.V. It is open-source software and part of the Common
 * Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from
 * thread pools and TCP/IP components to control architectures and learning algorithms.
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless,
 * we personally strongly object against this software used by the military, in the
 * bio-industry, for animal experimentation, or anything that violates the Universal
 * Declaration of Human Rights.
 *
 * Copyright © 2010 Anne van Rossum <anne@almende.com>
 *
 * @author  Anne C. van Rossum
 * @date    Feb 17, 2011
 * @project Replicator FP7
 * @company Almende B.V.
 * @case
 */


#ifndef TCPSTREAM_H_
#define TCPSTREAM_H_

// General files
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

namespace yarp {
    namespace os {
        namespace impl {
            class TcpStream;
        }
    }
}

/* **************************************************************************************
 * Interface of TcpStream
 * **************************************************************************************/


class yarp::os::impl::TcpStream {
public:
    /**
     * Constructor TcpStream
     */
    TcpStream();

    /**
     * Destructor ~TcpStream
     */
    virtual ~TcpStream();

    inline ssize_t recv_n (void *buf, size_t n) {
        return ::recv(sd, buf, n, 0);
    }

    inline ssize_t recv_n (void *buf, size_t n, struct timeval *tv) {
        setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char *)tv, sizeof (*tv));
        return ::recv(sd, buf, n, 0);
    }

    inline ssize_t recv (void *buf, size_t n) {
        return ::recv(sd, buf, n, 0);
    }

    inline ssize_t recv (void *buf, size_t n, struct timeval *tv) {
        setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char *)tv, sizeof (*tv));
        return ::recv(sd, buf, n, 0);
    }

    inline ssize_t send_n (const void *buf, size_t n) {
        return ::send(sd, buf, n, 0);
    }

    inline ssize_t send_n (const void *buf, size_t n, struct timeval *tv) {
        setsockopt(sd, SOL_SOCKET, SO_SNDTIMEO, (char *)tv, sizeof (*tv));
        return ::send(sd, buf, n, 0);
    }

    // No idea what this should do...
    void flush() { }

    void close_reader() {
        if (sd!=-1) {
            ::shutdown(sd,SHUT_RD);
        }
    }

    void close_writer() {
        if (sd!=-1) {
            ::shutdown(sd,SHUT_WR);
        }
    }

    void close() {
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
    inline int set_option(int level, int option, void *optval, int optlen) const {
        return setsockopt(sd, level, option, (char*)optval, optlen);
    }

    // Wrapper around the getsockopt system call.
    inline int get_option(int level, int option, void *optval, int *optlen) const {
        return getsockopt(sd, level, option, (char*)optval, (socklen_t*)optlen);
    }
private:
    // stream descriptor
    int sd;
};

#endif /* TCPSTREAM_H_ */

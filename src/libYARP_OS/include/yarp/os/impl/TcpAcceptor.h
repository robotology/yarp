/**
 * @file TcpAcceptor.h
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
 * @date    Feb 15, 2011
 * @project Replicator FP7
 * @company Almende B.V.
 * @case
 */


#ifndef TCPACCEPTOR_H_
#define TCPACCEPTOR_H_


#include <yarp/os/impl/TwoWayStream.h>
#include <yarp/os/impl/IOException.h>
#include <yarp/os/impl/Logger.h>

#include <yarp/os/impl/TcpStream.h>

//#include <sys/time.h>
//#include <iostream>
//
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <arpa/inet.h>
//#include <unistd.h>

// General files

namespace yarp {
    namespace os {
        namespace impl {
            class TcpAcceptor;
        }
    }
}

/* **************************************************************************************
 * Interface of TcpAcceptor
 * **************************************************************************************/

class yarp::os::impl::TcpAcceptor {
public:

    TcpAcceptor();

    virtual ~TcpAcceptor() {};

    int open(const Address& address);

    int connect(const Address& address);

    int close();

    int accept(TcpStream &new_stream);

//    const Address& getLocalAddress();

//    const Address& getRemoteAddress();
protected:
    int shared_open(const Address& address);

    int get_handle() { return ad; }

    void set_handle(int h) { ad = h; }
private:
    // acceptor descriptor
    int ad;
};

#endif /* TCPACCEPTOR_H_ */

/**
 * @file TcpConnector.h
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


#ifndef YARP_OS_IMPL_TCPCONNECTOR_H
#define YARP_OS_IMPL_TCPCONNECTOR_H

#include <yarp/os/Contact.h>
#include <yarp/os/impl/TcpStream.h>


namespace yarp {
    namespace os {
        namespace impl {
            class TcpConnector;
        }
    }
}

/* **************************************************************************************
 * Interface of TcpConnector
 * **************************************************************************************/

class yarp::os::impl::TcpConnector
{
public:
    /**
     * Constructor TcpConnector
     */
    TcpConnector();

    /**
     * Destructor ~TcpConnector
     */
    virtual ~TcpConnector();

    int connect (TcpStream &new_stream,
                 const yarp::os::Contact &remote_address);
protected:

    int open(TcpStream &stream);
private:

};

#endif // YARP_OS_IMPL_TCPCONNECTOR_H

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
 * @author 	Anne C. van Rossum
 * @date	Feb 17, 2011
 * @project	Replicator FP7
 * @company	Almende B.V.
 * @case	
 */


#ifndef TCPCONNECTOR_H_
#define TCPCONNECTOR_H_

#include <yarp/os/Contact.h>
#include <yarp/os/impl/TcpStream.h>
// General files
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <sys/wait.h>
//#include <signal.h>
//#include <string.h>
//#include <unistd.h>

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

class yarp::os::impl::TcpConnector {
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

#endif /* TCPCONNECTOR_H_ */

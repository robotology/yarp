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
 * @author 	Anne C. van Rossum
 * @date	Feb 17, 2011
 * @project	Replicator FP7
 * @company	Almende B.V.
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

	inline ssize_t recv (void *buf, size_t n, int flags) {
		return ::recv(sd, buf, n, flags);
	}

	inline ssize_t send_n (const void *buf, size_t n, int flags) {
		return ::send(sd, buf, n, flags);
	}

	// No idea what this should do...
	void flush() { ; }

	void close_reader() { 
	  
	}

	void close_writer() {
	}

	void close() {
	  if (sd!=-1) {
	    ::close(sd);
	  }
	}

	int open();

	int get_local_addr (sockaddr &);

	int get_remote_addr (sockaddr &);

	// get stream descriptor
	int get_handle() { return sd; }

	// set stream descriptor
	void set_handle(int h) { sd = h; }

protected:
private:
	// stream descriptor
	int sd;

};

#endif /* TCPSTREAM_H_ */

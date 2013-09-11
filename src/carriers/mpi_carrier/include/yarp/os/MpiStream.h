// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef _YARP_MPISTREAM_
#define _YARP_MPISTREAM_

#include <yarp/os/all.h>

#include <yarp/os/TwoWayStream.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/NetType.h>
#include <yarp/os/MpiComm.h>

#include <string>
#include <iostream>


namespace yarp {
    namespace os {
        class MpiStream;
    }
}


/**
 * Abstract base class for port communication via MPI.
 */
class yarp::os::MpiStream : public TwoWayStream, public InputStream, public OutputStream {
protected:
    int readAvail, readAt;
    char* readBuffer;
    bool terminate;
    ConstString name;
    yarp::os::MpiComm* comm;

    yarp::os::Contact local, remote;
public:


    MpiStream(ConstString name, MpiComm* comm);
    virtual ~MpiStream();
    virtual void close() = 0;
    virtual bool isOk();
    virtual void interrupt();
    virtual ssize_t read(const Bytes& b) = 0;
    virtual void write(const Bytes& b) = 0;
    virtual InputStream& getInputStream();
    virtual OutputStream& getOutputStream();
    virtual const yarp::os::Contact& getLocalAddress();
    virtual const yarp::os::Contact& getRemoteAddress();
    void resetBuffer();
    virtual void reset() { resetBuffer();}
    virtual void beginPacket();
    virtual void endPacket();


};


#endif // _YARP_MPISTREAM_


/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MPISTREAM
#define YARP_MPISTREAM

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

    using yarp::os::OutputStream::write;
    using yarp::os::InputStream::read;

    virtual void close() override = 0;
    virtual bool isOk() override;
    virtual void interrupt() override;
    virtual ssize_t read(const Bytes& b) override = 0;
    virtual void write(const Bytes& b) override = 0;
    virtual InputStream& getInputStream() override;
    virtual OutputStream& getOutputStream() override;
    virtual const yarp::os::Contact& getLocalAddress() override;
    virtual const yarp::os::Contact& getRemoteAddress() override;
    void resetBuffer();
    virtual void reset() override { resetBuffer();}
    virtual void beginPacket() override;
    virtual void endPacket() override;


};


#endif // YARP_MPISTREAM

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/all.h>

#include <yarp/os/impl/TwoWayStream.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/impl/Protocol.h>

#include <string>
#include <iostream>

#include "mpi.h"

namespace yarp {
    namespace os {
        namespace impl {
            class MpiStream;
        }
    }
}

/**
 *
 * Send data via MPI.
 *
 */
class yarp::os::impl::MpiStream : public TwoWayStream, public InputStream, public OutputStream {
private:
    MPI_Comm intercomm, Port;
    int rank;
    char port_name[MPI_MAX_PORT_NAME];
    int readAvail, readAt;
    char* readBuffer;
public:
    MpiStream(bool server=false);
    ~MpiStream();
    void connect(String port);
    void accept();
    char* getPortName();
    virtual void close();
    virtual bool isOk();
    virtual void interrupt();
    virtual int read(const Bytes& b);
    virtual void write(const Bytes& b);
    virtual InputStream& getInputStream();
    virtual OutputStream& getOutputStream();
    virtual const Address& getLocalAddress();
    virtual const Address& getRemoteAddress();
    virtual void reset();
    virtual void beginPacket();
    virtual void endPacket();

private:
    Address local, remote;
};



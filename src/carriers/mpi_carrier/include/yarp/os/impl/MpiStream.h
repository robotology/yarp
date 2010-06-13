// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
 * Send data via MPI.
 *
 * Uses the dynamic process management from the MPI-2 standard to
 * set up a communicator between the two processes.
 *
 * @bug No proper disconnection.
 * @warning Seems to work, but still experimental.
 */
class yarp::os::impl::MpiStream : public TwoWayStream, public InputStream, public OutputStream {
private:
    MPI_Comm intercomm, initial_comm;
    // MPI_Comm Port;
    int rank;
    char port_name[MPI_MAX_PORT_NAME];
    char unique_id[10+MPI_MAX_PROCESSOR_NAME];
    int readAvail, readAt;
    char* readBuffer;
    static int stream_counter;
    bool terminate;
    String name;
public:
    MpiStream(String name, bool server=false);
    ~MpiStream();
    bool connect(String port);
    bool accept();
    /**
     * Check if both ports are within the same process.
     */
    void checkLocal(String other);
    String getUID() { return String(unique_id);}
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

    void increase_counter();
    void decrease_counter();

private:
    Address local, remote;
};



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

#include <yarp/os/impl/TwoWayStream.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/Semaphore.h>

#include <string>
#include <iostream>

#include "mpi.h"

namespace yarp {
    namespace os {
        namespace impl {
            class MpiStream;
            class MpiComm;
        }
    }
}

/**
 * Wrapper for MPI_Comm communicator.
 */
class yarp::os::impl::MpiComm {
    String name;
    static bool isInit;
    static bool isThreadSafe;
public:
    char port_name[MPI_MAX_PORT_NAME];
    char unique_id[10+MPI_MAX_PROCESSOR_NAME];
    MPI_Comm comm;
    yarp::os::Semaphore sema;


    MpiComm(String name);
    ~MpiComm() {
        MPI_Comm_disconnect(&comm);
    }
    bool connect(String port);
    bool accept();
    bool notLocal(String other);
    void initialize();

    static bool usable() {
        return MpiComm::isInit && MpiComm::isThreadSafe;
    }

    void openPort() {
        MPI_Open_port(MPI_INFO_NULL, port_name);
    }
    void closePort() {
        MPI_Close_port(port_name);
    }
    int rank() {
        int rank;
        MPI_Comm_rank(comm, &rank);
        return rank;
    }
};


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
protected:
    int readAvail, readAt;
    char* readBuffer;
    bool terminate;
    String name;
    yarp::os::impl::MpiComm* comm;

    Address local, remote;
public:


    MpiStream(String name, MpiComm* comm);
    virtual ~MpiStream();
    virtual void close();
    virtual bool isOk();
    virtual void interrupt();
    virtual int read(const Bytes& b) = 0;
    virtual void write(const Bytes& b) = 0;
    virtual InputStream& getInputStream();
    virtual OutputStream& getOutputStream();
    virtual const Address& getLocalAddress();
    virtual const Address& getRemoteAddress();
    virtual void reset();
    virtual void beginPacket();
    virtual void endPacket();


};


#endif // _YARP_MPISTREAM_


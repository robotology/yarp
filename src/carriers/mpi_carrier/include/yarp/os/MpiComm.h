/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2011 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef YARP_MPICOMM
#define YARP_MPICOMM

#include <yarp/os/all.h>

#include <yarp/os/ConstString.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Thread.h>

#include <string>
#include <iostream>

#include "mpi.h"


namespace yarp {
    namespace os {
        class MpiComm;
        class MpiControlThread;
    }
}

class yarp::os::MpiControlThread : public yarp::os::Thread {
    bool terminate;
public:
    MpiControlThread() : terminate(false) {}
    void finalize() {
        terminate = true;}
    bool threadInit() override;
    void run() override {
        while (!terminate) {SystemClock::delaySystem(1);}
    }
    void threadRelease() override;
};

extern yarp::os::MpiControlThread *MpiControl;



/**
 * Wrapper for MPI_Comm communicator.
 *
 * Uses the dynamic process management from the MPI-2 standard to
 * set up a communicator between two seperate processes.
 *
 * @note Needs an MPI implementation with THREAD_MULTIPLE support.
 */
class yarp::os::MpiComm {
    ConstString name;

public:
    char port_name[MPI_MAX_PORT_NAME];
    char unique_id[10+MPI_MAX_PROCESSOR_NAME];
    MPI_Comm comm;
    yarp::os::Semaphore sema;


    MpiComm(ConstString name);
    ~MpiComm() {
        #ifdef MPI_DEBUG
        printf("[MpiComm @ %s] Destructor\n", name.c_str() );
        #endif
        MPI_Comm_disconnect(&comm);
    }
    bool connect(ConstString port);
    bool accept();
    void disconnect(bool disconn);
    bool notLocal(ConstString other);

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


#endif  // _YARP_MPICOMM_


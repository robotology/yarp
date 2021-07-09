/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MPICOMM_H
#define YARP_MPICOMM_H

#include <string>
#include <yarp/os/Semaphore.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/Thread.h>

#include <string>
#include <iostream>

#include <mpi.h>

#include "MpiLogComponent.h"

class MpiControlThread :
        public yarp::os::Thread
{
    bool terminate;
public:
    MpiControlThread() : terminate(false) {}
    void finalize() {
        terminate = true;}
    bool threadInit() override;
    void run() override {
        while (!terminate) {yarp::os::SystemClock::delaySystem(1);}
    }
    void threadRelease() override;
};

extern MpiControlThread *MpiControl;



/**
 * Wrapper for MPI_Comm communicator.
 *
 * Uses the dynamic process management from the MPI-2 standard to
 * set up a communicator between two separate processes.
 *
 * @note Needs an MPI implementation with THREAD_MULTIPLE support.
 */
class MpiComm
{
    std::string name;

public:
    char port_name[MPI_MAX_PORT_NAME];
    char unique_id[10+MPI_MAX_PROCESSOR_NAME];
    MPI_Comm comm;
    yarp::os::Semaphore sema;


    MpiComm(std::string name);
    ~MpiComm() {
        yCTrace(MPI_CARRIER, "[MpiComm @ %s] Destructor", name.c_str() );
        MPI_Comm_disconnect(&comm);
    }
    bool connect(std::string port);
    bool accept();
    void disconnect(bool disconn);
    bool notLocal(std::string other);

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


#endif // YARP_MPICOMM_H

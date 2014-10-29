// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2011 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/MpiComm.h>
#include <yarp/os/Log.h>
#include <yarp/os/NetType.h>
#include <mpi.h>

#include <stdlib.h>
#include <unistd.h>

using namespace yarp::os;

/* --------------------------------------- */
/* MpiControlThread */

yarp::os::MpiControlThread *MpiControl = NULL;

void finalizeMPI(void) {
    if (MpiControl) {
        MpiControl->finalize();
        delete MpiControl;
        MpiControl = NULL;
    }
    int ct = 0;
    int finalized;
    while (ct < 5) {
        sleep(1);
        MPI_Finalized(&finalized);
        if (finalized) {
            return;
        }
        ct++;
    }
    yError("MpiControlThread: Finalizing MPI failed! Calling MPI_Abort");
    MPI_Abort(MPI_COMM_WORLD,1);
}

void MpiControlThread::threadRelease() {
    yInfo("MpiControlThread: Trying to finalize MPI...");
    MPI_Finalize();
    yInfo("MpiControlThread: Successfully finalized MPI...");
}

bool MpiControlThread::threadInit() {
    // We have to finalize MPI at process termination
    atexit(finalizeMPI);

    #ifdef MPI_DEBUG
    printf("[MpiControl] Initialize\n");
    #endif

    int provided;
    // We need full multithread support for MPI
    int requested = MPI_THREAD_MULTIPLE;
    // Passing NULL for argc/argv pointers is fine for MPI-2
    int err = MPI_Init_thread(NULL, NULL, requested , &provided);
    if (err != MPI_SUCCESS ) {
        yError("MpiControlThread: Couldn't initialize MPI\n");
        return false;
    }

    if (provided >= requested) {
        return true;
    }
    else {
        MPI_Finalize();
        yError("MpiControlThread: MPI implementation doesn't provide required thread safety: requested %s, provided %s", NetType::toString(requested).c_str(), NetType::toString(provided).c_str());
        return false;
    }
}


/* --------------------------------------- */
/* MpiComm */

MpiComm::MpiComm(ConstString name) : name(name) {
    if (MpiControl == NULL) {
        MpiControl = new yarp::os::MpiControlThread;
    }
    if (! MpiControl->isRunning()) {
        MpiControl->start();
    }

    // Complicated way of doing comm = MPI_COMM_SELF;
    // but safer
    MPI_Group self_group;
    MPI_Comm_group( MPI_COMM_SELF, &self_group );
    MPI_Comm_create( MPI_COMM_SELF, self_group, &comm );


    // Create a unique identifier to prevent intra-process use of MPI
    int length = 0;
    MPI_Get_processor_name(unique_id, &length);
    sprintf(unique_id + length, "____pid____%d", getpid());
    #ifdef MPI_DEBUG
    printf("[MpiComm @ %s] Unique id: %s\n", name.c_str(), unique_id);
    #endif
}

//TODO: replace by static variable check??!?
bool MpiComm::notLocal(ConstString other) {
    if (other == ConstString(unique_id)) {
        yError("MPI does not support process local communication\n");
        return false;
    }
    return true;
}

bool MpiComm::connect(ConstString port) {

    char* port_name = new char[port.length()+1];
    memcpy(port_name, port.c_str(), port.length());
    port_name[port.length()] = '\0';

    #ifdef MPI_DEBUG
    printf("[MpiComm @ %s] Waiting for accept\n", name.c_str());
    #endif

    MPI_Comm intercomm;
    MPI_Comm_set_errhandler(comm, MPI_ERRORS_RETURN);
    int err = MPI_Comm_connect( port_name, MPI_INFO_NULL, 0, comm,  &intercomm );
    MPI_Comm_set_errhandler(comm, MPI_ERRORS_ARE_FATAL);

    if (err != MPI_SUCCESS ) {
        yError("MpiCarrier: Couldn't create connection\n");
        return false;
    }


    #ifdef MPI_DEBUG
    printf("[MpiComm @ %s] Connection established\n", name.c_str());
    #endif

    bool high = true;
    MPI_Intercomm_merge(intercomm, high, &comm);
    MPI_Comm_disconnect(&intercomm);

    #ifdef MPI_DEBUG
    printf("[MpiComm @ %s] Comms merged \n", name.c_str());
    #endif

    delete[] port_name;

    return true;
}
bool MpiComm::accept() {
    #ifdef MPI_DEBUG
    printf("[MpiComm @ %s] Waiting for connect\n", name.c_str());
    #endif

    MPI_Comm intercomm, newintra;
    MPI_Comm_accept( port_name, MPI_INFO_NULL, 0, comm, &intercomm );

    #ifdef MPI_DEBUG
    printf("[MpiComm @ %s] Connection accepted \n", name.c_str());
    #endif

    bool high = false;
    // Complicated way of doing comm = Merge(intercomm)
    // but necessary
    MPI_Intercomm_merge(intercomm, high, &newintra);
    MPI_Comm_disconnect(&intercomm);
    MPI_Comm_disconnect(&comm);
    comm = newintra;

    #ifdef MPI_DEBUG
    printf("[MpiComm @ %s] Comms merged \n", name.c_str());
    #endif

    return true;
}


void MpiComm::disconnect(bool disconn) {
    #ifdef MPI_DEBUG
    printf("[MpiComm @ %s] split from group : %d \n", name.c_str(), disconn);
    #endif
    MPI_Comm new_comm;
    MPI_Comm_split(comm, disconn, rank(), &new_comm);
    MPI_Comm_disconnect(&comm);
    comm = new_comm;
    #ifdef MPI_DEBUG
    printf("[MpiComm @ %s] new rank : %d \n", name.c_str(), rank());
    #endif
}


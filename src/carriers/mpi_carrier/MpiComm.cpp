/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "MpiComm.h"

#include <yarp/os/Log.h>
#include <yarp/os/NetType.h>
#include <mpi.h>

#include <cstdlib>
#include <utility>
#include <unistd.h>

using namespace yarp::os;

/* --------------------------------------- */
/* MpiControlThread */

MpiControlThread *MpiControl = nullptr;

void finalizeMPI() {
    if (MpiControl) {
        MpiControl->finalize();
        delete MpiControl;
        MpiControl = nullptr;
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
    yCError(MPI_CARRIER, "MpiControlThread: Finalizing MPI failed! Calling MPI_Abort");
    MPI_Abort(MPI_COMM_WORLD,1);
}

void MpiControlThread::threadRelease() {
    yCInfo(MPI_CARRIER, "MpiControlThread: Trying to finalize MPI...");
    MPI_Finalize();
    yCInfo(MPI_CARRIER,"MpiControlThread: Successfully finalized MPI...");
}

bool MpiControlThread::threadInit() {
    // We have to finalize MPI at process termination
    atexit(finalizeMPI);

    yCDebug(MPI_CARRIER,"[MpiControl] Initialize");

    int provided;
    // We need full multithread support for MPI
    int requested = MPI_THREAD_MULTIPLE;
    // Passing NULL for argc/argv pointers is fine for MPI-2
    int err = MPI_Init_thread(nullptr, nullptr, requested , &provided);
    if (err != MPI_SUCCESS ) {
        yCError(MPI_CARRIER, "MpiControlThread: Couldn't initialize MPI");
        return false;
    }

    if (provided >= requested) {
        return true;
    }
    else {
        MPI_Finalize();
        yCError(MPI_CARRIER, "MpiControlThread: MPI implementation doesn't provide required thread safety: requested %s, provided %s", yarp::conf::numeric::to_string(requested).c_str(), yarp::conf::numeric::to_string(provided).c_str());
        return false;
    }
}


/* --------------------------------------- */
/* MpiComm */

MpiComm::MpiComm(std::string name) :
        name(std::move(name))
{
    if (MpiControl == nullptr) {
        MpiControl = new MpiControlThread;
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
    yCDebug(MPI_CARRIER, "[MpiComm @ %s] Unique id: %s", name.c_str(), unique_id);
}

//TODO: replace by static variable check??!?
bool MpiComm::notLocal(std::string other) {
    if (other == std::string(unique_id)) {
        yCError(MPI_CARRIER, "MPI does not support process local communication");
        return false;
    }
    return true;
}

bool MpiComm::connect(std::string port) {

    char* port_name = new char[port.length()+1];
    memcpy(port_name, port.c_str(), port.length());
    port_name[port.length()] = '\0';

    yCDebug(MPI_CARRIER, "[MpiComm @ %s] Waiting for accept", name.c_str());

    MPI_Comm intercomm;
    MPI_Comm_set_errhandler(comm, MPI_ERRORS_RETURN);
    int err = MPI_Comm_connect( port_name, MPI_INFO_NULL, 0, comm,  &intercomm );
    MPI_Comm_set_errhandler(comm, MPI_ERRORS_ARE_FATAL);

    if (err != MPI_SUCCESS ) {
        yCError(MPI_CARRIER, "MpiCarrier: Couldn't create connection");
        return false;
    }

    yCDebug(MPI_CARRIER, "[MpiComm @ %s] Connection established", name.c_str());

    bool high = true;
    MPI_Intercomm_merge(intercomm, high, &comm);
    MPI_Comm_disconnect(&intercomm);

    yCDebug(MPI_CARRIER, "[MpiComm @ %s] Comms merged", name.c_str());

    delete[] port_name;

    return true;
}
bool MpiComm::accept() {
    yCDebug(MPI_CARRIER, "[MpiComm @ %s] Waiting for connect", name.c_str());

    MPI_Comm intercomm, newintra;
    MPI_Comm_accept( port_name, MPI_INFO_NULL, 0, comm, &intercomm );

    yCDebug(MPI_CARRIER, "[MpiComm @ %s] Connection accepted", name.c_str());

    bool high = false;
    // Complicated way of doing comm = Merge(intercomm)
    // but necessary
    MPI_Intercomm_merge(intercomm, high, &newintra);
    MPI_Comm_disconnect(&intercomm);
    MPI_Comm_disconnect(&comm);
    comm = newintra;

    yCDebug(MPI_CARRIER, "[MpiComm @ %s] Comms merged", name.c_str());

    return true;
}


void MpiComm::disconnect(bool disconn) {
    yCDebug(MPI_CARRIER, "[MpiComm @ %s] split from group : %d", name.c_str(), disconn);
    MPI_Comm new_comm;
    MPI_Comm_split(comm, disconn, rank(), &new_comm);
    MPI_Comm_disconnect(&comm);
    comm = new_comm;
    yCDebug(MPI_CARRIER, "[MpiComm @ %s] new rank : %d", name.c_str(), rank());
}

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifdef CREATE_MPI_CARRIER

#include <yarp/os/impl/MpiStream.h>
#include <yarp/os/impl/Logger.h>
#include <mpi.h>

using namespace yarp::os::impl;


/* --------------------------------------- */
/* MpiComm */


bool MpiComm::isInit = false;
bool MpiComm::isThreadSafe = false;

MpiComm::MpiComm(String name) : name(name) {
    initialize();

    MPI_Group self_group;
    MPI_Comm_group( MPI_COMM_SELF, &self_group );
    MPI_Comm_create( MPI_COMM_SELF, self_group, &comm );
    // complicated (but maybe safer) way of doing
    // comm = MPI_COMM_SELF;

    // Create a unique identifier to prevent intra-process use of MPI
    int length = 0;
    MPI_Get_processor_name(unique_id, &length);
    sprintf(unique_id + length, "____pid____%d", getpid());
    #ifdef MPI_DEBUG
    printf("[MpiComm @ %s] Unique id: %s\n", name.c_str(), unique_id);
    #endif
}

//TODO: replace by static variable check??!?
bool MpiComm::notLocal(String other) {
    if (other == String(unique_id)) {
        YARP_LOG_ERROR("MPI does not support process local communication\n");
        return false;
    }
    return true;
}

bool MpiComm::connect(String port) {

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
        YARP_LOG_ERROR("MpiCarrier: Couldn't create connection\n");
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

    MPI_Comm intercomm;
    MPI_Comm_accept( port_name, MPI_INFO_NULL, 0, comm, &intercomm );

    #ifdef MPI_DEBUG
    printf("[MpiComm @ %s] Connection accepted \n", name.c_str());
    #endif

    bool high = false;
    MPI_Intercomm_merge(intercomm, high, &comm);
    MPI_Comm_disconnect(&intercomm);

    #ifdef MPI_DEBUG
    printf("[MpiComm @ %s] Comms merged \n", name.c_str());
    #endif

    return true;
}


void MpiComm::disconnect(bool disconn) {
    #ifdef MPI_DEBUG
    printf("[MpiComm @ %s] disconnect color : %d \n", name.c_str(), disconn);
    #endif
    MPI_Comm new_comm;
    MPI_Comm_split(comm, disconn, rank(), &new_comm);
    MPI_Comm_disconnect(&comm);
    comm = new_comm;
    #ifdef MPI_DEBUG
    printf("[MpiComm @ %s] new rank : %d \n", name.c_str(), rank());
    #endif
}




void MpiComm::initialize() {
    // We need to initialize MPI
    if (! MpiComm::isInit) {
        #ifdef MPI_DEBUG
        printf("[MpiComm @ %s] Initialize\n", name.c_str());
        #endif

        int provided;
        // We need full multithread support for MPI
        int requested = MPI_THREAD_MULTIPLE;
        // Passing NULL for argc/argv pointers is fine for MPI-2
        int err = MPI_Init_thread(NULL, NULL, requested , &provided);
        if (err == MPI_SUCCESS )
            MpiComm::isInit = true;
        else {
            YARP_LOG_ERROR("MpiComm: Couldn't initialize MPI\n");
            return;
        }

        if (provided >= requested) {
            MpiComm::isThreadSafe = true;
        }
        else {
            YARP_SPRINTF2(Logger::get(),error, "MpiComm: MPI implementation doesn't provide required thread safety: requested %d, provided %d\n", requested, provided);
        }
    }
}

/* --------------------------------------- */
/* MpiStream */






MpiStream::MpiStream(String n, MpiComm* c)
    : terminate(false), name(n), comm(c) {
    readBuffer = NULL;
    resetBuffer();

}
MpiStream::~MpiStream() {
    #ifdef MPI_DEBUG
    printf("[MpiStream @ %s] Destructor called\n", name.c_str());
    #endif
}

void MpiStream::resetBuffer() {
    // reset buffer
    readAt = 0;
    readAvail = 0;
    delete [] readBuffer;
    readBuffer = NULL;
}

bool MpiStream::isOk() {
    return !terminate;
}

 void MpiStream::interrupt() {
    #ifdef MPI_DEBUG
    printf("[MpiStream @ %s] Trying to interrupt\n", name.c_str());
    #endif
    terminate = true;
}

/* --------------------------------------- */
/* TwoWayStream */

InputStream& MpiStream::getInputStream() {
    return *this;
}
OutputStream& MpiStream::getOutputStream() {
    return *this;
}
const Address& MpiStream::getLocalAddress() {
    // left undefined
    return local;
}
const Address& MpiStream::getRemoteAddress() {
    // left undefined
    return remote;
}
void MpiStream::beginPacket() {
     // nothing to do
}
void MpiStream::endPacket() {
     // nothing to do
}



#else

extern "C" int MpiStreamStatus() {
    return 0;
}

#endif

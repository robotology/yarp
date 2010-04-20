// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifdef CREATE_MPI_CARRIER

#include <yarp/os/impl/MpiStream.h>

using namespace yarp::os::impl;


int MpiStream::stream_counter = 0;

MpiStream::MpiStream(String n, bool server) : terminate(false), name(n) {
    readBuffer = NULL;
    reset();

    // Port = MPI_COMM_SELF;
    increase_counter();

    // Create a unique identifier to prevent intra-process use of MPI
    int length = 0;
    MPI_Get_processor_name(unique_id, &length);
    sprintf(unique_id + length, "____pid____%d", getpid());
#ifdef MPI_DEBUG
    printf("[MpiStream @ %s] Unique id: %s\n", name.c_str(), unique_id);
#endif

   initial_comm = MPI_COMM_SELF;
    if (server) {
        MPI_Open_port(MPI_INFO_NULL, port_name);
    }
}
MpiStream::~MpiStream() {
#ifdef MPI_DEBUG
    printf("[MpiStream @ %s] ", name.c_str());
    printf("Destructor called\n");
#endif
    decrease_counter();
#ifdef MPI_DEBUG
    printf("[MpiStream @ %s] ", name.c_str());
    printf("Destructed!!!\n");
#endif
}

void MpiStream::increase_counter() {
#ifdef MPI_DEBUG
    printf("[MpiStream @ %s] ", name.c_str());
    printf("Increase stream counter from %d\n", MpiStream::stream_counter);
#endif
    // We need to initialize MPI
    if (MpiStream::stream_counter == 0) {
        char*** mpi_carrier_argv = NULL;
        int mpi_carrier_argc[1] = {0};
        MPI_Init(mpi_carrier_argc, mpi_carrier_argv);
    }
    MpiStream::stream_counter++;
}

void MpiStream::decrease_counter() {
#ifdef MPI_DEBUG
    printf("[MpiStream @ %s] ", name.c_str());
    printf("Decrease stream counter from %d\n", MpiStream::stream_counter);
#endif
    MpiStream::stream_counter--;
    if (MpiStream::stream_counter == 0) {
#ifdef MPI_DEBUG
        printf("[MpiStream] ");
        printf("Stream counter equals 0; calling MPI_Finalize... ");
#endif
        MPI_Finalize();
#ifdef MPI_DEBUG
        printf("Done\n");
#endif
    }
    
}
void MpiStream::checkLocal(String other) {
    printf("!!!!!!!!!! ----- !!!!!!!!!!!\n");
    printf("!!!!!!!!!!! %s = ", unique_id);
    printf("%s", String(unique_id).c_str());
    if (other == String(unique_id)) {
        printf("ERROR: MpiStream does not support process local communication\n");
        throw;
    }
}

bool MpiStream::connect(String port) {
    memcpy(port_name, port.c_str(), port.length());        
    port_name[port.length()] = '\0';
#ifdef MPI_DEBUG
    printf("[MpiStream @ %s] ", name.c_str());
    printf("Waiting for accept\n");
#endif
    // TODO: check for connection failure
    MPI_Comm_connect( port_name, MPI_INFO_NULL, 0, initial_comm,  &intercomm ); 
#ifdef MPI_DEBUG
    printf("[MpiStream @ %s] ", name.c_str());
    printf("Connection established\n");
#endif
    //MPI_Intercomm_merge(intercomm, true, &Port);
    //Port = intercomm;

    return true;
}
bool MpiStream::accept() {
#ifdef MPI_DEBUG
    printf("[MpiStream @ %s] ", name.c_str());
    printf("Waiting for connect\n");
#endif
    // TODO: check for connection failure
    MPI_Comm_accept( port_name, MPI_INFO_NULL, 0, initial_comm, &intercomm );
#ifdef MPI_DEBUG
    printf("[MpiStream @ %s] ", name.c_str());
    printf("Connection accepted \n");
#endif
    MPI_Close_port(port_name);
    //MPI_Intercomm_merge( intercomm, false, &Port);
    //Port = intercomm;

    return true;
}
char* MpiStream::getPortName() {
    return port_name;
}

void MpiStream::close() {
    // nothing to do
#ifdef MPI_DEBUG
    printf("[MpiStream @ %s] ", name.c_str());
    printf("Trying to close; calling MPI disconnect\n");
#endif
    // to be protected by mutex???
    terminate = true;
    MPI_Comm_disconnect(&intercomm);
#ifdef MPI_DEBUG
    printf("[MpiStream @ %s] ", name.c_str());
    printf("Successfully disconnected\n");
#endif
}

bool MpiStream::isOk() {
    // no diagnostics for MPI; works or terminates
    return true;
}

 void MpiStream::interrupt() {
#ifdef MPI_DEBUG
    printf("[MpiStream @ %s] ", name.c_str());
    printf("Trying to interrupt\n");
#endif
    close();
}

/////////////////////////////////////////////////
// InputStream

int MpiStream::read(const Bytes& b) {
    if (readAvail == 0) {
        // get new data
        reset();
        int size;
        int available = 0;
        int tag = 0;
        MPI_Status status;
        //MPI_(&size, 1, MPI_INT, 0, intercomm);
        while (!available) {
            if (terminate)
                return 0;
            MPI_Iprobe(0, tag, intercomm, &available, &status);
        }
        MPI_Get_count(&status, MPI_BYTE, &size);
        if (size == b.length()) {
            // size of received data matches expected data
            // do not use buffer, but write directly
            MPI_Recv(b.get(), size, MPI_BYTE, 0, tag, intercomm, &status);
            return size;
        }
        else {
            // allocate new buffer
            readBuffer = new char[size];
            MPI_Recv(readBuffer, size, MPI_BYTE, 0, tag, intercomm, &status);
            //printf("got new msg of size %d\n", size);
            readAvail = size;
            readAt = 0;
        }
    }
    if (readAvail>0) {
        // copy data from buffer to destination object
        int take = readAvail;
        if (take>b.length()) {
            take = b.length();
        }
        memcpy(b.get(),readBuffer+readAt,take);
        //printf("read %d of %d \n", take, readAvail);
        readAt += take;
        readAvail -= take;
        return take;
    }
    return 0;
}

/////////////////////////////////////////////////
// OutputStream

void MpiStream::write(const Bytes& b) {
    int size = b.length();
    //MPI_Bcast(&size, 1, MPI_INT, MPI_ROOT, intercomm );
    MPI_Request request;
    MPI_Status status;
    int flag = 0;
    MPI_Isend(b.get(), size, MPI_BYTE, 0, 0, intercomm, &request );
    while(flag == 0) {
        if (terminate)
            break;
        MPI_Test(&request, &flag, &status);
    }
}

/////////////////////////////////////////////////
// TwoWayStream

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
void MpiStream::reset() {
    // reset buffer
    readAt = 0;
    readAvail = 0;
    delete [] readBuffer;
    readBuffer = NULL;
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

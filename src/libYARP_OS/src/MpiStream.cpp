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

MpiStream::MpiStream(bool server) : terminate(false) {
    readBuffer = NULL;
    reset();
    Port = MPI_COMM_SELF;
    if (server) {
        MPI_Open_port(MPI_INFO_NULL, port_name);
    }
}
MpiStream::~MpiStream() {
    printf("STREAM dtor\n");
    decrease_counter();
    printf("stream dying \n");
}

void MpiStream::increase_counter() {
    printf("inc stream counter: %d\n", MpiStream::stream_counter);
    // We need to initialize MPI
    if (MpiStream::stream_counter == 0) {
        char*** mpi_carrier_argv = NULL;
        int mpi_carrier_argc[1] = {0};
        MPI_Init(mpi_carrier_argc, mpi_carrier_argv);
    }
    MpiStream::stream_counter++;
}

void MpiStream::decrease_counter() {
    printf("dec stream counter: %d\n", MpiStream::stream_counter);
    MpiStream::stream_counter--;
    if (MpiStream::stream_counter == 0) {
        MPI_Finalize();
    }
    
}


void MpiStream::connect(String port) {
    memcpy(port_name, port.c_str(), port.length());        
    port_name[port.length()] = '\0';
    MPI_Comm_connect( port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF,  &intercomm ); 
    //MPI_Intercomm_merge(intercomm, true, &Port);
    //Port = intercomm;
}
void MpiStream::accept() {
    MPI_Comm_accept( port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &intercomm );
    //MPI_Intercomm_merge( intercomm, false, &Port);
    //Port = intercomm;
}
char* MpiStream::getPortName() {
    return port_name;
}

 void MpiStream::close() {
    // nothing to do
    printf("disconnecting\n");
    // to be protected by mutex???
    terminate = true;
    MPI_Comm_disconnect(&intercomm);
    printf("bye bye\n");
}

bool MpiStream::isOk() {
    // no diagnostics for MPI; works or terminates
    return true;
}

 void MpiStream::interrupt() {
     printf("try interupt\n");
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
    MPI_Send(b.get(), size, MPI_BYTE, 0, 0, intercomm );
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

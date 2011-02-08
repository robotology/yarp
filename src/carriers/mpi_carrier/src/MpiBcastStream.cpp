// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifdef CREATE_MPI_CARRIER

#include <yarp/os/impl/MpiBcastStream.h>

using namespace yarp::os::impl;


/////////////////////////////////////////////////
// InputStream

int MpiBcastStream::read(const Bytes& b) {
    if (readAvail == 0) {
        // get new data
        reset();
        int size;
        MPI_Bcast(&size, 1, MPI_INT, 0,comm->comm);
        if (size == -1) {
            comm->accept();
            return 0;
        }
        if (size == b.length()) {
            // size of received data matches expected data
            // do not use buffer, but write directly
            MPI_Bcast(b.get(), size, MPI_BYTE, 0, comm->comm);
            return size;
        }
        else {
            // allocate new buffer
            readBuffer = new char[size];
            MPI_Bcast(readBuffer, size, MPI_BYTE, 0, comm->comm);
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

void MpiBcastStream::write(const Bytes& b) {
    comm->sema.wait();
    int size = b.length();
    MPI_Bcast(&size, 1, MPI_INT, 0, comm->comm );
    MPI_Bcast(b.get(), size, MPI_BYTE, 0, comm->comm );
    comm->sema.post();
}


#endif

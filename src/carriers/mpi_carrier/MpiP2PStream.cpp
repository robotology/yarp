/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "MpiP2PStream.h"

using namespace yarp::os;


/////////////////////////////////////////////////
// InputStream

ssize_t MpiP2PStream::read(Bytes& b) {
    if (readAvail == 0) {
        // get new data
        reset();
        int size;
        int available = 0;
        int tag = 0;
        int rank = comm->rank();
        MPI_Status status;
        while (true) {
            if (terminate) {
                return -1;
            }
            // Check for a message
            MPI_Iprobe(!rank, tag, comm->comm, &available, &status);
            if (available) {
                break;
            }
            // Prevent the busy polling which hurts
            // performance in the oversubscription scenario
            Time::yield();
        }
        MPI_Get_count(&status, MPI_BYTE, &size);
        if (size == (int)b.length()) {
            // size of received data matches expected data
            // do not use buffer, but write directly
            MPI_Recv(b.get(), size, MPI_BYTE, !rank, tag, comm->comm, &status);
            return size;
        }
        else {
            // allocate new buffer
            readBuffer = new char[size];
            MPI_Recv(readBuffer, size, MPI_BYTE, !rank, tag, comm->comm, &status);
            yCDebug(MPI_CARRIER, "got new msg of size %d", size);
            readAvail = size;
            readAt = 0;
        }
    }
    if (readAvail>0) {
        // copy data from buffer to destination object
        int take = readAvail;
        if (take>(int)b.length()) {
            take = (int)b.length();
        }
        memcpy(b.get(),readBuffer+readAt,take);
        yCDebug(MPI_CARRIER, "read %d of %d", take, readAvail);
        readAt += take;
        readAvail -= take;
        return take;
    }
    return 0;
}

/////////////////////////////////////////////////
// OutputStream

void MpiP2PStream::write(const Bytes& b) {
    int size = b.length();
    //MPI_Bcast(&size, 1, MPI_INT, MPI_ROOT, intercomm );
    MPI_Request request;
    MPI_Status status;
    int flag = 0;
    int rank = comm->rank();
    //MPI_Send(b.get(), size, MPI_BYTE, 0, 0, intercomm);

    MPI_Isend(b.get(), size, MPI_BYTE, !rank , 0, comm->comm, &request );
    while(true) {
        /*
        // TODO: Need to implement a mechanism for breaking!!
        if (terminate)
            break;
        */
        // Check if message has been received
        MPI_Test(&request, &flag, &status);
        if (flag) {
            break;
        }
        // Prevent the busy polling which hurts
        // performance in the oversubscription scenario
        Time::yield();
    }
}

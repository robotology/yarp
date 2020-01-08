/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "MpiBcastStream.h"

using namespace yarp::os;


void MpiBcastStream::startJoin() {
    comm->sema.wait();
    int cmd = CMD_JOIN;
    MPI_Bcast(&cmd, 1, MPI_INT, 0,comm->comm);
}



// Connection commands

void MpiBcastStream::execCmd(int cmd) {
  switch (cmd) {
    case CMD_JOIN:
      // Connect:
      // Let a new port join the broadcast group
      comm->accept();
      break;
    case CMD_DISCONNECT:
      // Disconnect:
      // Let a port leave the broadcast group
      int length;
      MPI_Bcast(&length, 1, MPI_INT, 0,comm->comm);
      char* remote = new char[length];
      MPI_Bcast(remote, length, MPI_CHAR, 0,comm->comm);
      terminate = !strcmp(remote, name.c_str());
      #ifdef MPI_DEBUG
      printf("[MpiBcastStream @ %s] Got disconnect : %s => %d\n", name.c_str(), remote, terminate);
      #endif
      delete [] remote;
      comm->disconnect(terminate);
      break;
  }
}



/////////////////////////////////////////////////
// InputStream

ssize_t MpiBcastStream::read(Bytes& b) {
    if (terminate) {
      return -1;
    }
    if (readAvail == 0) {
        // get new data
        reset();
        int size;
        #ifdef MPI_DEBUG
        printf("[MpiBcastStream @ %s] Trying to read\n", name.c_str());
        #endif

        MPI_Bcast(&size, 1, MPI_INT, 0,comm->comm);
        #ifdef MPI_DEBUG
        printf("[MpiBcastStream @ %s] got size %d\n", name.c_str(), size);
        #endif
        if (size < 0) {
            execCmd(size);
            return 0;
        }
        if ((size_t)size == b.length()) {
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
        if (take>(int)b.length()) {
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
    #ifdef MPI_DEBUG
    printf("[MpiBcastStream @ %s] getting sema for write\n", name.c_str());
    #endif
    comm->sema.wait();

    #ifdef MPI_DEBUG
    printf("[MpiBcastStream @ %s] trying to write\n", name.c_str());
    #endif
    int size = b.length();
    MPI_Bcast(&size, 1, MPI_INT, 0, comm->comm );
    MPI_Bcast((void*)b.get(), size, MPI_BYTE, 0, comm->comm );
    comm->sema.post();

    #ifdef MPI_DEBUG
    printf("[MpiBcastStream @ %s] done writing\n", name.c_str());
    #endif
}

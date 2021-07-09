/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * SPDX-License-Identifier: BSD-3-Clause
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
      yCDebug(MPI_CARRIER, "[MpiBcastStream @ %s] Got disconnect : %s => %d", name.c_str(), remote, terminate);
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
        yCDebug(MPI_CARRIER, "[MpiBcastStream @ %s] Trying to read", name.c_str());

        MPI_Bcast(&size, 1, MPI_INT, 0,comm->comm);
        yCDebug(MPI_CARRIER, "[MpiBcastStream @ %s] got size %d", name.c_str(), size);
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
            yCDebug(MPI_CARRIER, "got new msg of size %d", size);
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
        yCDebug(MPI_CARRIER, "read %d of %d", take, readAvail);
        readAt += take;
        readAvail -= take;
        return take;
    }
    return 0;
}

/////////////////////////////////////////////////
// OutputStream

void MpiBcastStream::write(const Bytes& b) {
    yCDebug(MPI_CARRIER, "[MpiBcastStream @ %s] getting sema for write", name.c_str());
    comm->sema.wait();

    yCDebug(MPI_CARRIER, "[MpiBcastStream @ %s] trying to write", name.c_str());
    int size = b.length();
    MPI_Bcast(&size, 1, MPI_INT, 0, comm->comm );
    MPI_Bcast((void*)b.get(), size, MPI_BYTE, 0, comm->comm );
    comm->sema.post();

    yCDebug(MPI_CARRIER, "[MpiBcastStream @ %s] done writing", name.c_str());
}

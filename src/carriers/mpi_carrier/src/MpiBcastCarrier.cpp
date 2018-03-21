/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/MpiBcastCarrier.h>
#include <yarp/os/Network.h>
#include <yarp/os/Log.h>

using namespace yarp::os;

MpiBcastCarrier::~MpiBcastCarrier() {
    #ifdef MPI_DEBUG
    printf("[MpiBcastCarrier @ %s] Destructor\n", name.c_str());
    #endif
}

void MpiBcastCarrier::close() {
    #ifdef MPI_DEBUG
    printf("[MpiBcastCarrier @ %s] Closing carrier \n", name.c_str() );
    #endif
    if (electionMember) {
        getCaster().remove(name, this);
        MpiBcastCarrier* elect = getCaster().getElect(name);
        if (elect == NULL) {
            delete comm;
        }
    } else {
        delete comm;
    }
}

void MpiBcastCarrier::createStream(bool sender) {
    if (sender) {
        MpiBcastCarrier* elect = getCaster().getElect(name);
        if (elect != NULL) {
            comm = elect->comm;
        }
        else {
            comm = new MpiComm(name+"->bcast");
        }
        stream = new MpiBcastStream(name+"->bcast", comm);
        MpiBcastStream *mpiStream = dynamic_cast<MpiBcastStream*> (stream);
        if(mpiStream)
            mpiStream->startJoin();
        getCaster().add(name, this);
        electionMember = true;
    } else {
        comm = new MpiComm(route);
        stream = new MpiBcastStream(route, comm);
    }

}

void MpiBcastCarrier::prepareDisconnect() {
    comm->sema.wait();
    #ifdef MPI_DEBUG
    printf("[MpiBcastCarrier @ %s] Disconnect : %s\n", name.c_str(), other.c_str());
    #endif
    int cmd = CMD_DISCONNECT;
    MPI_Bcast(&cmd, 1, MPI_INT, 0,comm->comm);
    int length = other.length() + name.length() + 3;
    char* remote_c = new char[length];
    strcpy(remote_c, (other+"<-"+name).c_str());
    MPI_Bcast(&length, 1, MPI_INT, 0,comm->comm);
    MPI_Bcast(remote_c, length, MPI_CHAR, 0,comm->comm);
    delete [] remote_c;
    comm->disconnect(false);
    comm->sema.post();

        //dynamic_cast<MpiBcastStream*> (stream)->disconnect(other);
    }



/*
 * Adopted from MCastCarrier
 * ----------------------------
 */
ElectionOf<yarp::os::PeerRecord<MpiBcastCarrier> > *MpiBcastCarrier::caster = NULL;

ElectionOf<yarp::os::PeerRecord<MpiBcastCarrier> >& MpiBcastCarrier::getCaster() {
    yarp::os::NetworkBase::lock();
    if (caster==NULL) {
        caster = new ElectionOf<yarp::os::PeerRecord<MpiBcastCarrier> >;
        yarp::os::NetworkBase::unlock();
        if (caster==NULL) {
            yError("No memory for MpiBcastCarrier::caster");
            std::exit(1);
        }
    } else {
        yarp::os::NetworkBase::unlock();
    }
    return *caster;
}
bool MpiBcastCarrier::isElect() {
    MpiBcastCarrier *elect = getCaster().getElect(name);
    return elect==this || elect==NULL;
}

bool MpiBcastCarrier::isActive() {
    return isElect();
}

/*
 * ----------------------------
 */

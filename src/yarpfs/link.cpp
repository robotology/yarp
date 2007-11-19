// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Giacomo Spigler
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <fuse/fuse.h>
//#include <fuse/fuse_lowlevel.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <yarp/os/all.h>
#include <yarp/NameClient.h>
#include <yarp/NameConfig.h>

#include <string>
#include <signal.h>

#include <ace/Containers_T.h>

#include "yarputils.h"
#include "yarpfns.h"


int yarp_readlink(const char *path, char *buf, size_t size) {



    return 0;
}


int yarp_unlink(const char *path) {



    return 0;
}


int yarp_symlink(const char *from, const char *to) {
    //TODO: actually, it only works with ln -s /read rd, and yet it throwns
    //  some errors

    //YPath ypath(from);
    //if (!ypath.isStem()) { //Check that the path exists? Is it right?
    //    return -ENOENT;
    //}


    //Create the new Contact
    Contact src = Network::queryName(from);

    printf("source [%s] is %s\n", from, src.toString().c_str());

    Contact dest = Contact::byName(to).addSocket(src.getCarrier(),src.getHost(),src.getPort());

    printf("dest [%s] should be %s\n", to, src.toString().c_str());

    Network::registerContact(dest);


    return 0;
}


int yarp_link(const char *from, const char *to) {
    //TODO: will it ever be possible to hard link ports?
    //  If possible, it might be an alias for yarp_simlink, as with YARP 
    //  there isn't a sym/hard linking difference

    //Create the new Contact
    Contact src = Network::queryName(from);

    printf("source [%s] is %s\n", from, src.toString().c_str());

    Contact dest = Contact::byName(to).addSocket(src.getCarrier(),src.getHost(),src.getPort());

    printf("dest [%s] should be %s\n", to, src.toString().c_str());


    return 0;
}



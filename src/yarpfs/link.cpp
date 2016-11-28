/*
 * Copyright (C) 2007 RobotCub Consortium, Giacomo Spigler
 * Authors: Paul Fitzpatrick, Giacomo Spigler
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <fuse/fuse.h>
//#include <fuse/fuse_lowlevel.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <yarp/os/all.h>
#include <yarp/os/impl/NameConfig.h>

#include <string>
#include <signal.h>

#include <ace/Containers_T.h>

#include "yarputils.h"
#include "yarpfns.h"


int yarp_readlink(const char *path, char *buf, size_t size) {

    YPath ypath(path);
    if (!ypath.isSymLink()) { 
        return -ENOENT;
    }

    memcpy(buf,ypath.getLink().c_str(),size);

    return 0;
}


int yarp_unlink(const char *path) {



    return 0;
}


int yarp_symlink(const char *to, const char *from) {
    //TODO: actually, it only works with ln -s /read rd, and yet it throwns
    //  some errors

    //YPath ypath(from);
    //if (!ypath.isStem()) { //Check that the path exists? Is it right?
    //    return -ENOENT;
    //}

    //Create the new Contact
    //Contact src = Network::queryName(from);
    //printf("source [%s] is %s\n", from, src.toString().c_str());
    //Contact dest(to, src.getCarrier(), src.getHost(), src.getPort());
    //printf("dest [%s] should be %s\n", to, src.toString().c_str());
    //Network::registerContact(dest);

    printf("SYMLINK requested from [%s] to [%s]\n", from, to);

    // special symlink entry
    //Contact src = Network::queryName(to);
    Contact dest(from, "symlink", "none", 1);
    printf("Planning to register %s / %d / %d\n", 
           dest.toString().c_str(),
           dest.isValid(),
           dest.getPort());
    Network::registerContact(dest);
    Network::setProperty(from,"link",Value(to));

    return 0;
}


int yarp_link(const char *from, const char *to) {
    //TODO: will it ever be possible to hard link ports?
    //  If possible, it might be an alias for yarp_simlink, as with YARP 
    //  there isn't a sym/hard linking difference

    //Create the new Contact
    Contact src = Network::queryName(from);

    printf("source [%s] is %s\n", from, src.toString().c_str());

    Contact dest(to, src.getCarrier(), src.getHost(), src.getPort());

    printf("dest [%s] should be %s\n", to, src.toString().c_str());


    return 0;
}



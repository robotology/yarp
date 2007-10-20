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


static int yarp_readlink(const char *path, char *buf, size_t size) {
/*
    int res;

    res = readlink(path, buf, size - 1);
    if (res == -1)
        return -errno;

    buf[res] = '\0';
*/
    return 0;
}


static int yarp_unlink(const char *path) {
/*
    int res;

    res = unlink(path);
    if (res == -1)
        return -errno;
*/
    return 0;
}


static int yarp_symlink(const char *from, const char *to) {
/*
    int res;

    res = symlink(from, to);
    if (res == -1)
        return -errno;
*/
    return 0;
}


static int yarp_link(const char *from, const char *to) {
/*
    int res;

    res = link(from, to);
    if (res == -1)
        return -errno;
*/
    return 0;
}



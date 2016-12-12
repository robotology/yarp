/*
 * Copyright (C) 2007 RobotCub Consortium
 * Author: Alessandro Scalzo <alessandro.scalzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_SHMEMTYPES_H
#define YARP_OS_IMPL_SHMEMTYPES_H

#define SHMEM_DEFAULT_SIZE 4096

struct ShmemHeader_t
{
    bool resize;
    bool close;

    int size;
    int newsize;

    int head;
    int tail;
    int avail;
    int waiting;
};

struct ShmemPacket_t
{
    int command;
    int size;
};

#endif // YARP_OS_IMPL_SHMEMTYPES_H

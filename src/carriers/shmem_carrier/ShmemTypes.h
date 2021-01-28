/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SHMEM_SHMEMTYPES_H
#define YARP_SHMEM_SHMEMTYPES_H

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

#endif // YARP_SHMEM_SHMEMTYPES_H

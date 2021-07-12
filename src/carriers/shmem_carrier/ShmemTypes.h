/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Alessandro Scalzo alessandro@liralab.it
 * Copyright (C) 2007 RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef SHMEM_TYPES_H
#define SHMEM_TYPES_H

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

#endif

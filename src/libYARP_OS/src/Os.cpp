// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author: Lorenzo natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/Os.h>

#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_unistd.h> 
#include <ace/OS_NS_signal.h>
#include <ace/OS_NS_stdlibh>

void *yarp::os::signal(int signum, void (*sighandler)(int))
{
	switch (signum)
	{
		case yarp::os::YARP_SIGINT:
			return ACE_OS::signal(SIGINT, (ACE_SignalHandler) sighandler);
		case yarp::os::YARP_SIGTERM:
			return ACE_OS::signal(SIGTERM, (ACE_SignalHandler) sighandler);
		default:
			return 0; //signal not implemented yet
	}
}

void yarp::os::exit(int v)
{
	ACE_OS::exit(v);
}

const char *yarp::os::getenv(const char *var)
{
	return ACE_OS::getenv(var);
}

int yarp::os::mkdir(const char *p)
{
	return ACE_OS::mkdir(p);
}

int yarp::os::stat(const char *path)
{
	ACE_stat dummy;
	return ACE_OS::stat(path, &dummy);
}

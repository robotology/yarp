// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

///
/// $Id: YARPScheduler.cpp,v 1.3 2006-10-24 16:43:50 eshuy Exp $
///
///

///
#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

///
#ifdef __WIN32__
#include <windows.h>
#include <mmsystem.h>
#endif

#ifdef Yield
#undef Yield
#endif

#include <yarp/YARPScheduler.h>

void YARPScheduler::yield ()
{
	// wonder whether it's portable.
	ACE_Time_Value tv(0);
	ACE_OS::sleep(tv);
}

void YARPScheduler::setHighResScheduling ()
{
#ifdef ACE_WIN32
	TIMECAPS tm;
	timeGetDevCaps(&tm, sizeof(TIMECAPS));
	timeBeginPeriod(1);
#else
	ACE_DEBUG ((LM_DEBUG, "setHighResScheduling: Not implemented, perhaps it doesn't make sense on your OS\n"));
#endif
}

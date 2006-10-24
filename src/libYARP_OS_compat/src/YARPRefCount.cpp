// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

///
/// $Id: YARPRefCount.cpp,v 1.3 2006-10-24 16:43:50 eshuy Exp $
///
///

#include <yarp/YARPRefCount.h>

#ifdef YARPREFCOUNT_SINGLE_MUTEX
#include <yarp/YARPSemaphore.h>

#ifdef __WIN32__
/// library initialization.
#pragma init_seg(lib)
#endif

YARPSemaphore yref_sema(1);

#define YR_WAIT yref_sema.Wait()
#define YR_POST yref_sema.Post()
#endif

PYARPRefCount AddYarpRefCount(PYARPRefCount& ref)
{
	PYARPRefCount result = NULL;
	YR_WAIT;
	if (ref==NULL)
        {
            ref = new YARPRefCount;
        }

	ACE_ASSERT (ref!=NULL);
	
	ref->ref_count++;
	result = ref;
	YR_POST;
	return result;
}

YARPRefCount::YARPRefCount()
{
	ref_count = 1;
}

YARPRefCount::~YARPRefCount()
{
}

void YARPRefCount::AddRef()
{
	YR_WAIT;
	ref_count++;
	YR_POST;
}

void YARPRefCount::ZeroRef()
{
	YR_WAIT;
	ref_count = 0;
	YR_POST;
}

void YARPRefCount::RemoveRef()
{
	YR_WAIT;
	ACE_ASSERT (ref_count>0);
	ref_count--;
	if (ref_count==0)
        {
            YR_POST;
            Destroy();
        }
	else
        {
            YR_POST;
        }
}


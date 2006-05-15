// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, /// 
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// $Id: YARPRefCount.h,v 1.2 2006-05-15 15:57:58 eshuy Exp $
///
///
/*
  paulfitz Mon May 21 16:51:13 EDT 2001
*/

#ifndef YARPRefCounted_INC
#define YARPRefCounted_INC

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/Synch.h>
#include <ace/Log_Msg.h>

#include <yarp/YARPAll.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

// All the operations in YARPRefCounted need to be mutexed.

#define YARPREFCOUNT_SINGLE_MUTEX

#ifndef YARPREFCOUNT_SINGLE_MUTEX
#error "Code not written for per-ref mutex"
#endif

class YARPRefCount;

typedef YARPRefCount *PYARPRefCount;

PYARPRefCount AddYarpRefCount(PYARPRefCount& ref);


class YARPRefCount
{
protected:
	int ref_count;

public:
	YARPRefCount();
	virtual ~YARPRefCount();

	int GetReferenceCount() { return ref_count; }

	void AddRef();
	void RemoveRef();
	void ZeroRef();

	virtual void Destroy() {}

	friend PYARPRefCount AddYarpRefCount(PYARPRefCount& ref);
};

class YARPRefCounted : public YARPRefCount
{
public:
	virtual void Destroy() {}

	// returns NULL if only one user of object and always_clone is false, 
	// otherwise should return a copy of the object if possible
	virtual void *Clone(bool always_clone)
	{ 
		ACE_UNUSED_ARG(always_clone);
		return NULL; 
	}
};


class YARPRefCountedBuffer : public YARPRefCounted
{
protected:
	char *memory;
	int len;
	int owned;

public:
	YARPRefCountedBuffer() { memory = NULL; owned = 0; }
	YARPRefCountedBuffer(int nlen) 
	{
		memory = new char[nlen];
		ACE_ASSERT(memory!=NULL);
		owned = 1;
		len = nlen; 
	}

	YARPRefCountedBuffer(char *n_memory, int n_len, int n_owned = 0)
	{ 
		memory = n_memory;  
		len = n_len;  
		owned = n_owned; 
	}

	virtual ~YARPRefCountedBuffer()
	{
		if (memory!=NULL && owned)
            {
                delete[] memory;
            }
		memory = NULL;
	}  

	void Set(char *n_memory, int n_len, int n_owned = 0)
	{ 
		ACE_ASSERT(memory==NULL);  
		memory = n_memory;  len = n_len;  
		owned = n_owned; 
	}

	virtual void *Clone(int always_clone)
	{ 
		ACE_UNUSED_ARG(always_clone);
		int is_implemented=0; 
		ACE_ASSERT(is_implemented==1); 
		return NULL; 
	}

	char *GetRawBuffer() { return memory; }
	int GetLength() { return len; }
	void ForceLength(int n_len) { len = n_len; }
	int GetReferenceCount() { return ref_count; }
};


// Intended to be applied to RefCounted objects only
template <class T>
class YARPRefCountedPtr
{
public:
	T *ptr;

	YARPRefCountedPtr()          { ptr = NULL; }
	YARPRefCountedPtr(T *nptr)   { ptr = NULL;  Take(nptr); }

	virtual ~YARPRefCountedPtr() { Reset();    }

	T *Ptr() { return ptr; }

	void Set(T *nptr)
	{
		Reset();
		ptr = nptr;
		ACE_ASSERT(ptr!=NULL);
		ptr->AddRef();
	}

	void Take(T *nptr)
	{
		Reset();
		ptr = nptr;
	}

	void Reset()
	{ 
		if (ptr!=NULL) ptr->RemoveRef(); 
		ptr = NULL; 
	}

	/*
      void Switch(CountedPtr<T>& peer)
      {
      T *tmp = ptr;
      ptr = peer.ptr;
      peer.ptr = tmp;
      }
	*/

	void MakeIndependent()
	{
		int needed;
		T *nptr;
		if (ptr!=NULL)
            {
                nptr = (T*)ptr->Clone(false);
                if (nptr!=NULL)
                    {
                        Take(nptr);
                    }
            }
	}
};

#endif

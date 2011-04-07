// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PLATFORMVECTOR_
#define _YARP2_PLATFORMVECTOR_

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
#  include <ace/Vector_T.h>
#  define PlatformVector ACE_Vector
#  define PLATFORM_VECTOR_ITERATOR(x) ACE_Vector_Iterator<x >
#else
#  include <vector>
#  define PlatformVector std::vector
#  define PLATFORM_VECTOR_ITERATOR(x) std::vector<x >::iterator
#endif

#endif

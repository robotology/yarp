// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PLATFORMSET_
#define _YARP2_PLATFORMSET_

#include <yarp/conf/system.h>
#ifndef YARP_USE_STL
#include <ace/Containers_T.h>
#define PlatformMultiSet ACE_Ordered_MultiSet
#define PLATFORM_MULTISET_ITERATOR(x) ACE_Ordered_MultiSet_Iterator<x >
#else
#include <set>
#define PlatformMultiSet std::multiset
#define PLATFORM_MULTISET_ITERATOR(x) std::multiset<x >::iterator
#endif

#endif


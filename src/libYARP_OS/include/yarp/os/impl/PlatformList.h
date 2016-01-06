// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_PLATFORMLIST
#define YARP2_PLATFORMLIST

#include <yarp/conf/system.h>
#ifndef YARP_USE_STL
#  include <ace/Containers_T.h>
#  define PLATFORM_LIST(x) ACE_Double_Linked_List<x>
#  define PLATFORM_LIST_EMPTY(x) x.is_empty()
#  define PLATFORM_LIST_CLEAR(x) x.reset()
#  define PLATFORM_LIST_PUSH_BACK(x,y) x.insert_tail(y)
#  define PLATFORM_LIST_GET(x,y) x.get(y)
#else
#  include <list>
#  define PLATFORM_LIST(x) std::list<x *>
#  define PLATFORM_LIST_EMPTY(x) x.empty()
#  define PLATFORM_LIST_CLEAR(x) while (!x.empty()) { delete x.back(); x.pop_back(); }
#  define PLATFORM_LIST_PUSH_BACK(x,y) x.push_back(y)
#  define PLATFORM_LIST_GET(x,y) y = x.front()
#endif

#endif

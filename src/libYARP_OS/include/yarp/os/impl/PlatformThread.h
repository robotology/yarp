// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_PLATFORMTHREAD
#define YARP2_PLATFORMTHREAD

#include <yarp/conf/system.h>
#ifdef YARP_HAS_CXX11
#  include <thread>
#  define Platform_hthread_t std::thread
#  define Platform_thread_t size_t
#  define PLATFORM_THREAD_SELF() std::hash<std::thread::id>()(std::this_thread::get_id())
#  define PLATFORM_THREAD_RETURN unsigned
#  define PLATFORM_THREAD_JOIN(x) ([](std::thread& y) { if (!y.joinable()) return false; y.join(); return true; }(x))
#else
#  ifdef YARP_HAS_ACE
#    include <ace/Thread.h>
#    include <ace/Sched_Params.h>
#    define Platform_hthread_t ACE_hthread_t
#    define Platform_thread_t ACE_thread_t
#    define PLATFORM_THREAD_SELF() ACE_Thread::self()
#    define PLATFORM_THREAD_RETURN unsigned
#    define PLATFORM_THREAD_JOIN(x) ACE_Thread::join(x)
#  else
#    include <pthread.h>
#    define Platform_hthread_t pthread_t
#    define Platform_thread_t long int
#    define PLATFORM_THREAD_SELF() pthread_self()
#    define PLATFORM_THREAD_RETURN void *
#    define PLATFORM_THREAD_JOIN(x) pthread_join(x,0/*NULL*/)
#  endif
#endif

#endif

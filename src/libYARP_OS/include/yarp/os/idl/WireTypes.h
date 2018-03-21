/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IDL_WIRETYPES_H
#define YARP_OS_IDL_WIRETYPES_H


#if __cplusplus >= 201103L
# include <cstdint>
#elif defined(_WIN32) && !defined(__MINGW32__) && (!defined(_MSC_VER) || _MSC_VER<1600)
  typedef __int32 int32_t;
  typedef unsigned __int32 uint32_t;
  typedef __int64 int64_t;
  typedef unsigned __int64 uint64_t;
#else
# include <stdint.h>
#endif

#include <string>
#include <vector>
#include <map>
#include <set>
#include <cstdio>

#include <yarp/os/idl/WireWriter.h>
#include <yarp/os/idl/WireReader.h>
#include <yarp/os/idl/WirePortable.h>
#include <yarp/os/idl/WireState.h>
#include <yarp/os/idl/WireVocab.h>
#include <yarp/os/idl/Unwrapped.h>
#include <yarp/os/idl/BottleStyle.h>
#include <yarp/os/idl/BareStyle.h>
#include <yarp/os/Wire.h>
#include <yarp/os/NetInt16.h>
#include <yarp/os/NetUint16.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/NetUint32.h>
#include <yarp/os/NetInt64.h>
#include <yarp/os/NetUint64.h>
#include <yarp/os/NetFloat32.h>
#include <yarp/os/NetFloat64.h>
#include <yarp/os/Log.h>

#endif // YARP_OS_IDL_WIRETYPES_H

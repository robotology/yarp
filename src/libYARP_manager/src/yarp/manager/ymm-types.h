/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_YMMTYPES
#define YARP_MANAGER_YMMTYPES

namespace yarp {
namespace manager {


#define WITH_GEOMETRY
/*
* TODO: check for the proper namespace
*/

typedef enum __NodeType {
    MODULE,
    INPUTD,
    OUTPUTD,
    STREAM_PORT,
    SERVICE_PORT,
    EVENT_PORT,
    APPLICATION,
    RESOURCE,
    INOUTD,
//    BEHAVIOR,
//    GROUP,
    NODE_FILENAME,
    NODE_APPTEMPLATE,
    NODE_OTHER
} NodeType;

typedef enum __OS {
    LINUX,
    WINDOWS,
    MAC,
    OTHER
} OS;

typedef struct __GyPoint {
    double x;
    double y;
} GyPoint;


/*
typedef enum __Carrier {
    TCP,
    UDP,
    MCAST,
    SHMEM,
    TEXT,
    UNKNOWN
} Carrier;
*/


} // namespace yarp
} // namespace manager



/*
*  declaring debugging macros
*/
#ifdef YMM_DEBUG
     #include <iostream>
     #include <cassert>
     #define __ASSERT( _cond ) assert(_cond)
     #define __CHECK_NULLPTR(_ptr) \
             assert(_ptr); \
             if( !_ptr ) return 0;

     #define __DEBUG_MSG(msg) std::cout<<"YMM: "<<msg<<endl;

#else
     #define __ASSERT( _cond )
     #define __CHECK_NULLPTR(_ptr) \
             if( !_ptr ) return 0;

     #define __DEBUG_MSG(msg)

#endif //YMM_DEBUG


#endif // __YARP_MANAGER_YMMTYPES__

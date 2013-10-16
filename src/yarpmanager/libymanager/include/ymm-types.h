// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __YMMTYPES__
#define __YMMTYPES__

//namespace ymm {

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




/*
*  declaring debugging macros
*/
#ifdef YMM_DEBUG
     #include <iostream>
     #include <assert.h>
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

//} 

#endif //__YMMTYPES__

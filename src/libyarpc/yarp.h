// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef YET_ANOTHER_ROBOT_PLATFORM_INC
#define YET_ANOTHER_ROBOT_PLATFORM_INC

// leave hook for elaborating declarations
#define YARP_DECLARE(rt) rt

#ifdef __cplusplus
extern "C" {
#endif

    // opaque implementation of network
    typedef struct yarpNetworkStruct {
        void *implementation;
    } yarpNetwork;
    typedef yarpNetwork *yarpNetworkPtr;
    
    // opaque implementation of ports
    typedef struct yarpPortStruct {
        void *implementation;
    } yarpPort;
    typedef yarpPort *yarpPortPtr;
    
    // opaque implementation of contacts
    typedef struct yarpContactStruct {
        void *implementation;
    } yarpContact;
    typedef yarpContact *yarpContactPtr;

    // opaque implementation of bottles
    typedef struct yarpBottleStruct {
        void *implementation;
    } yarpBottle;
    typedef yarpBottle *yarpBottlePtr;
    
    // Network functions
    YARP_DECLARE(yarpNetworkPtr) yarpNetworkCreate();
    YARP_DECLARE(void) yarpNetworkFree(yarpNetworkPtr network);

    // Port functions
    YARP_DECLARE(yarpPortPtr) yarpPortCreate(yarpNetworkPtr network);
    YARP_DECLARE(void) yarpPortFree(yarpPortPtr port);
    YARP_DECLARE(int) yarpPortOpen(yarpPortPtr port, yarpContactPtr contact);
    YARP_DECLARE(int) yarpPortClose(yarpPortPtr port);

    // Contact functions
    YARP_DECLARE(yarpContactPtr) yarpContactCreate();
    YARP_DECLARE(int) yarpContactSetName(yarpContactPtr contact,
                                         const char *name);
    YARP_DECLARE(void) yarpContactFree(yarpContactPtr contact);


#ifdef __cplusplus
}
#endif

#endif



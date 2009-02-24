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

    typedef struct yarpReaderStruct {
        void *implementation;
    } yarpReader;
    typedef yarpReader *yarpReaderPtr;

    typedef struct yarpWriterStruct {
        void *implementation;
    } yarpWriter;
    typedef yarpWriter *yarpWriterPtr;

    typedef struct yarpPortableStruct {
        void *implementation;
        int (*write) (yarpWriterPtr connection);
        int (*read) (yarpReaderPtr connection);
        int (*onCompletion)();
        int (*onCommencement)();
    } yarpPortable;
    typedef yarpPortable *yarpPortablePtr;

    // Network functions
    YARP_DECLARE(yarpNetworkPtr) yarpNetworkCreate();
    YARP_DECLARE(void) yarpNetworkFree(yarpNetworkPtr network);
    YARP_DECLARE(int) yarpNetworkSetLocalMode(yarpNetworkPtr network,
                                              int isLocal);
    // if carrier is NULL, connection type is tcp
    YARP_DECLARE(int) yarpNetworkConnect(yarpNetworkPtr network, 
                                         const char *src,
                                         const char *dest,
                                         const char *carrier);
    YARP_DECLARE(int) yarpNetworkDisonnect(yarpNetworkPtr network, 
                                           const char *src,
                                           const char *dest);

    // Port functions
    YARP_DECLARE(yarpPortPtr) yarpPortCreate(yarpNetworkPtr network);
    YARP_DECLARE(yarpPortPtr) yarpPortCreateOpen(yarpNetworkPtr network,
                                                 const char *name);
    YARP_DECLARE(void) yarpPortFree(yarpPortPtr port);
    YARP_DECLARE(int) yarpPortOpen(yarpPortPtr port, const char *name);
    YARP_DECLARE(int) yarpPortOpenEx(yarpPortPtr port, yarpContactPtr contact);
    YARP_DECLARE(int) yarpPortClose(yarpPortPtr port);
    YARP_DECLARE(int) yarpPortEnableBackgroundWrite(yarpPortPtr port,
                                                    int writeInBackgroundFlag);
    YARP_DECLARE(int) yarpPortWrite(yarpPortPtr port, 
                                    yarpPortablePtr msg);
    YARP_DECLARE(int) yarpPortRead(yarpPortPtr port, 
                                   yarpPortablePtr msg,
                                   int willReply);
    YARP_DECLARE(int) yarpPortReply(yarpPortPtr port, 
                                    yarpPortablePtr msg);
    YARP_DECLARE(int) yarpPortWriteWithReply(yarpPortPtr port, 
                                             yarpPortablePtr msg,
                                             yarpPortablePtr reply);

    // Contact functions
    YARP_DECLARE(yarpContactPtr) yarpContactCreate();
    YARP_DECLARE(void) yarpContactFree(yarpContactPtr contact);
    YARP_DECLARE(int) yarpContactSetName(yarpContactPtr contact,
                                         const char *name);

    // Reader functions
    YARP_DECLARE(int) yarpReaderExpectInt(yarpReaderPtr reader, int *data);

    // Writer functions
    YARP_DECLARE(int) yarpWriterAppendInt(yarpWriterPtr c, int data);

    // Portable functions
    
    YARP_DECLARE(yarpPortablePtr) yarpPortableCreate();
    YARP_DECLARE(void) yarpPortableFree(yarpPortablePtr portable);
    YARP_DECLARE(int) yarpPortableSetWriteHandler(yarpPortablePtr portable, int (*write) (yarpWriterPtr connection));
    YARP_DECLARE(int) yarpPortableSetReadHandler(yarpPortablePtr portable, int (*read) (yarpReaderPtr connection));
    YARP_DECLARE(int) yarpPortableSetOnCompletionHandler(yarpPortablePtr portable, int(*onCompletion)());
    YARP_DECLARE(int) yarpPortableSetOnCommencementHandler(yarpPortablePtr portable, int(*onCommencement)());


#ifdef __cplusplus
}
#endif

#endif



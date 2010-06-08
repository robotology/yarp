// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008, 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 * This is a plain C interface to YARP.  It is as yet incomplete.
 *
 */

#ifndef YET_ANOTHER_ROBOT_PLATFORM_CVERSION_INC
#define YET_ANOTHER_ROBOT_PLATFORM_CVERSION_INC

#define YARP_DECLARE(return_type) return_type

#ifdef __cplusplus
extern "C" {
#endif

    /**
     *
     * Plain C network structure.  This structure represents the
     * overall YARP network.  
     *
     */
    typedef struct yarpNetworkStruct {
        void *implementation;
    } yarpNetwork;
    typedef yarpNetwork *yarpNetworkPtr;
    
    /**
     *
     * Plain C port structure.  This structure represents individual
     * ports within a YARP network.
     *
     */
    typedef struct yarpPortStruct {
        void *implementation;
    } yarpPort;
    typedef yarpPort *yarpPortPtr;
    
    /**
     *
     * Plain C contact structure.  This structure represents how to
     * reach a single port within a YARP network.
     *
     */
    typedef struct yarpContactStruct {
        void *implementation;
    } yarpContact;
    typedef yarpContact *yarpContactPtr;

    /**
     *
     * Plain C reader structure.  This structure represents how to
     * read from a connection between two ports.
     *
     */
    typedef struct yarpReaderStruct {
        void *implementation;
    } yarpReader;
    typedef yarpReader *yarpReaderPtr;

    /**
     *
     * Plain C writer structure.  This structure represents how to
     * write to a connection between two ports.
     *
     */
    typedef struct yarpWriterStruct {
        void *implementation;
    } yarpWriter;
    typedef yarpWriter *yarpWriterPtr;

    /**
     *
     * Plain C portable callback structure.  This structure holds
     * callbacks for connecting between two ports.
     *
     */
    typedef struct yarpPortableCallbacksStruct {
        int (*write) (yarpWriterPtr connection, void *client);
        int (*read) (yarpReaderPtr connection, void *client);
        int (*onCompletion)(void *client);
        int (*onCommencement)(void *client);
    } yarpPortableCallbacks;
    typedef yarpPortableCallbacks *yarpPortableCallbacksPtr;

    /**
     *
     * Plain C portable structure.  This structure represents how to
     * serialize data to and from a connection between two ports.
     *
     */
    typedef struct yarpPortableStruct {
        void *adaptor;
        void *client;
        yarpPortableCallbacksPtr callbacks;
    } yarpPortable;
    typedef yarpPortable *yarpPortablePtr;

    /**
     *
     * Plain C string structure.  This structure represents strings.
     *
     */
    typedef struct yarpStringStruct {
        void *implementation;
    } yarpString;
    typedef yarpString *yarpStringPtr;
    
    /**
     *
     * Plain C semaphore structure.  This structure represents semaphores.
     *
     */
    typedef struct yarpSemaphoreStruct {
        void *implementation;
    } yarpSemaphore;
    typedef yarpSemaphore *yarpSemaphorePtr;
    

    YARP_DECLARE(yarpNetworkPtr) yarpNetworkCreate();
    YARP_DECLARE(void) yarpNetworkFree(yarpNetworkPtr network);
    YARP_DECLARE(int) yarpNetworkSetLocalMode(yarpNetworkPtr network,
                                              int isLocal);
    YARP_DECLARE(int) yarpNetworkConnect(yarpNetworkPtr network, 
                                         const char *src,
                                         const char *dest,
                                         const char *carrier);
    YARP_DECLARE(int) yarpNetworkDisconnect(yarpNetworkPtr network, 
                                            const char *src,
                                            const char *dest);


    YARP_DECLARE(yarpPortPtr) yarpPortCreate(yarpNetworkPtr network);
    YARP_DECLARE(yarpPortPtr) yarpPortCreateOpen(yarpNetworkPtr network,
                                                 const char *name);
    YARP_DECLARE(void) yarpPortFree(yarpPortPtr port);
    YARP_DECLARE(int) yarpPortOpen(yarpPortPtr port, const char *name);
    YARP_DECLARE(int) yarpPortOpenEx(yarpPortPtr port, yarpContactPtr contact);
    YARP_DECLARE(int) yarpPortClose(yarpPortPtr port);
    YARP_DECLARE(int) yarpPortEnableBackgroundWrite(yarpPortPtr port,
                                                    int writeInBackgroundFlag);
    YARP_DECLARE(int) yarpPortWrite(yarpPortPtr port, yarpPortablePtr msg);
    YARP_DECLARE(int) yarpPortRead(yarpPortPtr port, 
                                   yarpPortablePtr msg,
                                   int willReply);
    YARP_DECLARE(int) yarpPortReply(yarpPortPtr port, yarpPortablePtr msg);
    YARP_DECLARE(int) yarpPortWriteWithReply(yarpPortPtr port, 
                                             yarpPortablePtr msg,
                                             yarpPortablePtr reply);
    YARP_DECLARE(yarpContactPtr) yarpContactCreate();
    YARP_DECLARE(void) yarpContactFree(yarpContactPtr contact);
    YARP_DECLARE(int) yarpContactSetName(yarpContactPtr contact,
                                         const char *name);


    YARP_DECLARE(int) yarpReaderExpectBlock(yarpReaderPtr reader, const char *data, int len);
    YARP_DECLARE(int) yarpReaderExpectText(yarpReaderPtr reader,
                                           yarpStringPtr str,
                                           char terminal);
    YARP_DECLARE(int) yarpReaderExpectInt(yarpReaderPtr reader, int *data);
    YARP_DECLARE(int) yarpReaderExpectDouble(yarpReaderPtr reader, double *data);
    YARP_DECLARE(int) yarpReaderIsTextMode(yarpReaderPtr reader);

    YARP_DECLARE(int) yarpWriterAppendInt(yarpWriterPtr c, int data);

    YARP_DECLARE(int) yarpPortableInit(yarpPortablePtr portable);
    YARP_DECLARE(int) yarpPortableFini(yarpPortablePtr portable);
    YARP_DECLARE(int) yarpPortableCallbacksInit(yarpPortableCallbacksPtr callbacks);

    /*
    YARP_DECLARE(int) yarpPortableCallbacksSetWrite(yarpPortableCallbacksPtr portable, int (*write) (yarpWriterPtr connection, void *ptr));
    YARP_DECLARE(int) yarpPortableCallbacksSetRead(yarpPortableCallbacksPtr portable, int (*read) (yarpReaderPtr connection, void *ptr));
    YARP_DECLARE(int) yarpPortableCallbacksSetOnCompletion(yarpPortableCallbacksPtr portable, int(*onCompletion)(void *ptr));
    YARP_DECLARE(int) yarpPortableCallbacksSetOnCommencement(yarpPortableCallbacksPtr portable, int(*onCommencement)(void *ptr));
    */

    YARP_DECLARE(yarpStringPtr) yarpStringCreate();
    YARP_DECLARE(void) yarpStringFree(yarpStringPtr str);
    YARP_DECLARE(const char *) yarpStringToC(yarpStringPtr str);
    YARP_DECLARE(int) yarpStringFromC(yarpStringPtr str, const char *c);

    YARP_DECLARE(yarpSemaphorePtr) yarpSemaphoreCreate(int initial);
    YARP_DECLARE(void) yarpSemaphoreFree(yarpSemaphorePtr sema);
    YARP_DECLARE(int) yarpSemaphoreWait(yarpSemaphorePtr sema);
    YARP_DECLARE(int) yarpSemaphorePost(yarpSemaphorePtr sema);
    YARP_DECLARE(int) yarpSemaphoreCheck(yarpSemaphorePtr sema);


#ifdef __cplusplus
}
#endif

#endif



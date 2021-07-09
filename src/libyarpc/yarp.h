/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * This is a plain C interface to YARP.  It is as yet incomplete.
 */

#ifndef YET_ANOTHER_ROBOT_PLATFORM_CVERSION_INC
#define YET_ANOTHER_ROBOT_PLATFORM_CVERSION_INC

#if !defined(_WIN32)
#define YARP_DECLARE(return_type) return_type
#else
#ifdef yarpc_EXPORTS
#define YARP_DECLARE(return_type) return_type __declspec(dllexport)
#else
#define YARP_DECLARE(return_type) return_type __declspec(dllimport)
#endif
#endif

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
        void *unused1;
        void *unused2;
        void *unused3;
        void *unused4;
    } yarpPortableCallbacks;
    typedef yarpPortableCallbacks *yarpPortableCallbacksPtr;

    /**
     *
     * Plain C portable structure.  This structure represents how to
     * serialize data to and from a connection between two ports.
     *
     */
    typedef struct yarpPortableStruct {
        void *client;            // user pointer, will be untouched
        void *implementation;    // private
        void *adaptor;           // private
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
     * Plain C thread callback structure.
     *
     */
    typedef struct yarpThreadCallbacksStruct {
        int (*run) (void *client);
        void (*beforeStart) (void *client);
        void (*afterStart) (int success, void *client);
        void (*onStop) (void *client);
        int (*threadInit) (void *client);
        void (*threadRelease) (void *client);
        void *unused4;
        void *unused5;
    } yarpThreadCallbacks;
    typedef yarpThreadCallbacks *yarpThreadCallbacksPtr;

    /**
     *
     * Plain C thread structure.  This structure represents threads.
     *
     */
    typedef struct yarpThreadStruct {
        void *implementation;
        void *client;
        yarpThreadCallbacksPtr callbacks;
    } yarpThread;
    typedef yarpThread *yarpThreadPtr;

    /**
     *
     * Plain C semaphore structure.  This structure represents semaphores.
     *
     */
    typedef struct yarpSemaphoreStruct {
        void *implementation;
    } yarpSemaphore;
    typedef yarpSemaphore *yarpSemaphorePtr;

    /**
     *
     * Plain C bottle structure.  This structure represents YARP bottles.
     * Note that you can send/receive Bottle-formatted messages without
     * using this structure.
     *
     */
    typedef yarpPortable yarpBottle;
    typedef yarpBottle *yarpBottlePtr;


    YARP_DECLARE(yarpNetworkPtr) yarpNetworkCreate();
    YARP_DECLARE(void) yarpNetworkFree(yarpNetworkPtr network);
    YARP_DECLARE(yarpNetworkPtr) yarpNetworkGet();
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
    YARP_DECLARE(int) yarpReaderExpectInt32(yarpReaderPtr reader, int *data);
    YARP_DECLARE(int) yarpReaderExpectDouble(yarpReaderPtr reader, double *data);
    YARP_DECLARE(int) yarpReaderIsTextMode(yarpReaderPtr reader);

    YARP_DECLARE(int) yarpWriterAppendInt32(yarpWriterPtr c, int data);

    YARP_DECLARE(int) yarpPortableInit(yarpPortablePtr portable,
                                       yarpPortableCallbacksPtr callbacks);

    YARP_DECLARE(int) yarpPortableFini(yarpPortablePtr portable);

    YARP_DECLARE(int) yarpPortableCallbacksInit(yarpPortableCallbacksPtr callbacks);
    YARP_DECLARE(int) yarpPortableCallbacksComplete(yarpPortableCallbacksPtr callbacks);
    YARP_DECLARE(int) yarpPortableCallbacksInstall(yarpPortableCallbacksPtr callbacks);
    YARP_DECLARE(yarpPortableCallbacksPtr) yarpPortableCallbacksGet();

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

    YARP_DECLARE(int) yarpThreadCallbacksInit(yarpThreadCallbacksPtr callbacks);

    YARP_DECLARE(int) yarpThreadInit(yarpThreadPtr thread,
                                     yarpThreadCallbacksPtr callbacks);
    YARP_DECLARE(int) yarpThreadFini(yarpThreadPtr thread);
    YARP_DECLARE(int) yarpThreadStart(yarpThreadPtr thread);
    YARP_DECLARE(int) yarpThreadStop(yarpThreadPtr thread);
    YARP_DECLARE(int) yarpThreadIsStopping(yarpThreadPtr thread);

    YARP_DECLARE(void) yarpTimeDelay(double seconds);
    YARP_DECLARE(double) yarpTimeNow();
    YARP_DECLARE(void) yarpTimeYield();

    YARP_DECLARE(int) yarpBottleInit(yarpBottlePtr bottle);
    YARP_DECLARE(int) yarpBottleFini(yarpBottlePtr bottle);
    YARP_DECLARE(void) yarpBottleAddInt32(yarpBottlePtr bottle, int x);
    YARP_DECLARE(void) yarpBottleAddDouble(yarpBottlePtr bottle, double x);
    YARP_DECLARE(void) yarpBottleAddString(yarpBottlePtr bottle, const char *x);
    YARP_DECLARE(int) yarpBottleWrite(yarpBottlePtr bottle, yarpWriterPtr connection);
    YARP_DECLARE(int) yarpBottleRead(yarpBottlePtr bottle, yarpReaderPtr connection);
    YARP_DECLARE(int) yarpBottleToString(yarpBottlePtr bottle,
                                         yarpStringPtr result);

#ifdef __cplusplus
}
#endif

#endif

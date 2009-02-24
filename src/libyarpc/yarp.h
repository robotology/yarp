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
     * Plain C portable structure.  This structure represents how to
     * serialize data to and from a connection between two ports.
     *
     */
    typedef struct yarpPortableStruct {
        void *implementation;
        int (*write) (yarpWriterPtr connection);
        int (*read) (yarpReaderPtr connection);
        int (*onCompletion)();
        int (*onCommencement)();
    } yarpPortable;
    typedef yarpPortable *yarpPortablePtr;


    /**
     * Create a network.  There should typically be one of 
     * these in a program using YARP.  You need one of these in order
     * to create ports.
     */
    YARP_DECLARE(yarpNetworkPtr) yarpNetworkCreate();

    /**
     * Destroy a network.
     */
    YARP_DECLARE(void) yarpNetworkFree(yarpNetworkPtr network);


    /**
     * Put a network in local mode (no YARP name server needed).
     */
    YARP_DECLARE(int) yarpNetworkSetLocalMode(yarpNetworkPtr network,
                                              int isLocal);

    /**
     *
     * Connect two ports with a specified carrier (connection type).
     * If the carrier is NULL, the connection will be of type tcp.
     *
     */
    YARP_DECLARE(int) yarpNetworkConnect(yarpNetworkPtr network, 
                                         const char *src,
                                         const char *dest,
                                         const char *carrier);

    /**
     *
     * Disconnect two ports from each other.
     *
     */
    YARP_DECLARE(int) yarpNetworkDisconnect(yarpNetworkPtr network, 
                                            const char *src,
                                            const char *dest);


    /**
     *
     * Create a port.  Ports start in a dormant, inactive state.
     *
     */
    YARP_DECLARE(yarpPortPtr) yarpPortCreate(yarpNetworkPtr network);

    /**
     *
     * Create and open a port.  If the port fails to open, NULL will
     * be returned.
     *
     */
    YARP_DECLARE(yarpPortPtr) yarpPortCreateOpen(yarpNetworkPtr network,
                                                 const char *name);

    /**
     *
     * Destroy a port.
     *
     */
    YARP_DECLARE(void) yarpPortFree(yarpPortPtr port);


    /**
     *
     * Open a port, assigning it a name.
     *
     */
    YARP_DECLARE(int) yarpPortOpen(yarpPortPtr port, const char *name);


    /**
     *
     * Open a port, using advanced contact information.
     *
     */
    YARP_DECLARE(int) yarpPortOpenEx(yarpPortPtr port, yarpContactPtr contact);

    /**
     *
     * Close a port.
     *
     */
    YARP_DECLARE(int) yarpPortClose(yarpPortPtr port);

    /**
     *
     * Configure a port to write messages in the background, so calls to
     * write messages may return immediately.
     *
     */
    YARP_DECLARE(int) yarpPortEnableBackgroundWrite(yarpPortPtr port,
                                                    int writeInBackgroundFlag);

    /**
     *
     * Write a message to a port.  The write handler of the msg structure
     * will be called.
     *
     */
    YARP_DECLARE(int) yarpPortWrite(yarpPortPtr port, 
                                    yarpPortablePtr msg);


    /**
     *
     * Read a message from a port.  The read handler of the msg structure
     * will be called.  Set willReply to 1 if you intend to later
     * give a reply to the message.  You must then call yarpPortReply
     * before any other port methods.
     *
     */
    YARP_DECLARE(int) yarpPortRead(yarpPortPtr port, 
                                   yarpPortablePtr msg,
                                   int willReply);


    /**
     *
     * Give a reply to a previously read message.
     *
     */
    YARP_DECLARE(int) yarpPortReply(yarpPortPtr port, 
                                    yarpPortablePtr msg);


    /**
     *
     * Write a message to a port, then wait for a reply.
     *
     */
    YARP_DECLARE(int) yarpPortWriteWithReply(yarpPortPtr port, 
                                             yarpPortablePtr msg,
                                             yarpPortablePtr reply);

    /**
     *
     * Create an empty contact.
     *
     */
    YARP_DECLARE(yarpContactPtr) yarpContactCreate();

    /**
     *
     * Destroy a contact.
     *
     */
    YARP_DECLARE(void) yarpContactFree(yarpContactPtr contact);


    /**
     *
     * Set the port name of a contact.
     *
     */
    YARP_DECLARE(int) yarpContactSetName(yarpContactPtr contact,
                                         const char *name);

    /**
     *
     * Read an integer from a connection.
     *
     */
    YARP_DECLARE(int) yarpReaderExpectInt(yarpReaderPtr reader, int *data);

    /**
     *
     * Write an integer to a connection.
     *
     */
    YARP_DECLARE(int) yarpWriterAppendInt(yarpWriterPtr c, int data);

    /**
     *
     * Create a portable structure, that is a bundle of serialization-related
     * callbacks.
     *
     */
    YARP_DECLARE(yarpPortablePtr) yarpPortableCreate();

    /**
     *
     * Destroy a portable structure.
     *
     */
    YARP_DECLARE(void) yarpPortableFree(yarpPortablePtr portable);

    /**
     *
     * set the write handler of a portable structure.
     *
     */
    YARP_DECLARE(int) yarpPortableSetWriteHandler(yarpPortablePtr portable, int (*write) (yarpWriterPtr connection));

    /**
     *
     * set the read handler of a portable structure.
     *
     */
    YARP_DECLARE(int) yarpPortableSetReadHandler(yarpPortablePtr portable, int (*read) (yarpReaderPtr connection));

    /**
     *
     * set the onCompletion handler of a portable structure.
     *
     */
    YARP_DECLARE(int) yarpPortableSetOnCompletionHandler(yarpPortablePtr portable, int(*onCompletion)());

    /**
     *
     * set the onCommencement handler of a portable structure.
     *
     */
    YARP_DECLARE(int) yarpPortableSetOnCommencementHandler(yarpPortablePtr portable, int(*onCommencement)());


#ifdef __cplusplus
}
#endif

#endif



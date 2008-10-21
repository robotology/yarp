// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_CARRIER_
#define _YARP2_CARRIER_

#include <yarp/os/impl/ShiftStream.h>
#include <yarp/Bytes.h>
#include <yarp/os/impl/SizedWriter.h>

namespace yarp {
    namespace os {
        namespace impl {
            class Carrier;
            class Protocol;
        }
    }
}

/**
 *
 * A base class for connection types (tcp, mcast, shmem, ...).
 *
 */
class yarp::os::impl::Carrier {
public:


    /**
     *
     * Factory method.  Get a new object of the same type as this one.
     *
     * @return a new object of the same type as this one.
     *
     */
    virtual Carrier *create() = 0;

    /**
     *
     * Get the name of this carrier ("tcp", "mcast", "shmem", ...)
     *
     * @return the name of this carrier
     */
    virtual String getName() = 0;

    /**
     *
     * Given the first 8 bytes received on a connection, decide if
     * this is the right carrier type to use for the rest of the
     * connection.  For example the "text" carrier looks for the
     * 8-byte sequence "CONNECT ".  See the YARP protocol documentation
     * for all the sequences in use here.  In general you are free to
     * add your own.
     *
     * @param header a buffer holding the first 8 bytes received on the connection
     * @return true if this is the carrier to use.
     *
     */
    virtual bool checkHeader(const yarp::os::Bytes& header) = 0;

    /**
     *
     * Configure this carrier based on the first 8 bytes of the 
     * connection.  This will only be called if checkHeader passed.
     * @param header a buffer holding the first 8 bytes received on the connection
     *
     */
    virtual void setParameters(const yarp::os::Bytes& header) = 0;

    /**
     *
     * Provide 8 bytes describing this connection sufficiently to 
     * allow the other side of a connection to select it.
     * @param header a buffer to hold the first 8 bytes to send on a connection
     *
     */
    virtual void getHeader(const yarp::os::Bytes& header) = 0;


    /**
     *
     * Check if this carrier is connectionless (like udp, mcast)
     * or connection based (like tcp).  This flag is used
     * by YARP to determine whether the connection can carry RPC
     * traffic, that is, messages with replies.  Replies are not
     * supported by YARP on connectionless carriers.
     *
     * @return true if carrier is connectionless
     *
     */
    virtual bool isConnectionless() = 0;


    /**
     *
     * Check if reading is implemented for this carrier.
     *
     * @return true if carrier can read messages
     *
     */
    virtual bool canAccept() = 0;

    /**
     *
     * Check if writing is implemented for this carrier.
     *
     * @return true if carrier can write messages
     *
     */
    virtual bool canOffer() = 0;

    /**
     *
     * Check if carrier is textual in nature
     *
     * @return true if carrier is text-based
     *
     */
    virtual bool isTextMode() = 0;

    /**
     *
     * Check if carrier can encode administrative messages, as opposed
     * to just user data.  The word escape is used in the sense of
     * escape character or escape sequence here.
     *
     * @return true if carrier can encode administrative messages
     *
     */
    virtual bool canEscape() = 0;

    /**
     *
     * Check if carrier has flow control, requiring sent messages
     * to be acknowledged by recipient.
     *
     * @return true if carrier requires acknowledgement.
     *
     */
    virtual bool requireAck() = 0;

    /**
     *
     * This flag is used by YARP to determine whether the connection
     * can carry RPC traffic, that is, messages with replies.
     *
     * @return true if carrier supports replies
     *
     */
    virtual bool supportReply() = 0;

    /**
     *
     * Check if carrier operates within a single process.  In such a 
     * case, YARP connections may get completely reorganized in order
     * to optimize them.
     *
     * @return true if carrier will only operate within a single process
     *
     */
    virtual bool isLocal() = 0;


    // sender
    virtual bool prepareSend(Protocol& proto) = 0;
    virtual bool sendHeader(Protocol& proto) = 0;
    virtual bool expectReplyToHeader(Protocol& proto) = 0;
    virtual bool sendIndex(Protocol& proto) = 0;

    virtual bool write(Protocol& proto, SizedWriter& writer) = 0;

    // receiver
    virtual bool expectExtraHeader(Protocol& proto) = 0;
    virtual bool respondToHeader(Protocol& proto) = 0;
    virtual bool expectIndex(Protocol& proto) = 0;
    virtual bool expectSenderSpecifier(Protocol& proto) = 0;
    virtual bool sendAck(Protocol& proto) = 0;
    virtual bool expectAck(Protocol& proto) = 0;

    virtual bool isActive() = 0;

    virtual String toString() = 0;

    virtual void close() {
    }

};

#endif


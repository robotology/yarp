// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_CONNECTION_
#define _YARP2_CONNECTION_

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        class Connection;
        class NullConnection;
    }
}

class YARP_OS_API yarp::os::Connection {
public:

    /**
     * Destructor.
     */
    virtual ~Connection() {
    }

   /**
     * Check if carrier is textual in nature
     *
     * @return true if carrier is text-based
     */
    virtual bool isTextMode() = 0;

    /**
     * Check if carrier can encode administrative messages, as opposed
     * to just user data.  The word escape is used in the sense of
     * escape character or escape sequence here.
     *
     * @return true if carrier can encode administrative messages
     */
    virtual bool canEscape() = 0;

    /**
     * Check if carrier has flow control, requiring sent messages
     * to be acknowledged by recipient.
     *
     * @return true if carrier requires acknowledgement.
     */
    virtual bool requireAck() = 0;

    /**
     * This flag is used by YARP to determine whether the connection
     * can carry RPC traffic, that is, messages with replies.
     *
     * @return true if carrier supports replies
     */
    virtual bool supportReply() = 0;

    /**
     * Check if carrier operates within a single process.
     *
     * In such a case, YARP connections may get completely reorganized
     * in order to optimize them.
     *
     * @return true if carrier will only operate within a single process
     */
    virtual bool isLocal() = 0;


    /**
     * Check if carrier is "push" or "pull" style.
     *
     * Push means that the side that initiates a connection is also the
     * one that will sending of data or commands.  All native YARP
     * connections are like this.
     * A "pull" style is equivalent to automatically sending a "reverse
     * connection" port command at the start of the connection.
     *
     * @return true if carrier is "push" style, false if "pull" style
     */
    virtual bool isPush() = 0;

   /**
     * Check if this carrier is connectionless (like udp, mcast) or
     * connection based (like tcp).
     *
     * This flag is used by YARP to determine whether the connection can
     * carry RPC traffic, that is, messages with replies.  Replies are
     * not supported by YARP on connectionless carriers.
     *
     * @return true if carrier is connectionless
     */
    virtual bool isConnectionless() = 0;


    /**
     * Check if this carrier uses a broadcast mechanism.
     *
     * This flag is used to determine whether it is appropriate to send
     * "kill" messages using a carrier or whether they should be sent
     * "out-of-band"
     *
     * @return true if carrier uses a broadcast mechanism.
     */
    virtual bool isBroadcast() = 0;


    /**
     * Check if carrier is alive and error free.
     *
     * @return true if carrier is active.
     */
    virtual bool isActive() = 0;
};


class YARP_OS_API yarp::os::NullConnection : public Connection {
public:
    virtual bool isTextMode() { return true; }
    virtual bool canEscape() { return true; }
    virtual bool requireAck() { return false; }
    virtual bool supportReply() { return false; }
    virtual bool isLocal() { return false; }
    virtual bool isPush() { return true; }
    virtual bool isConnectionless() { return false; }
    virtual bool isBroadcast() { return false; }
    virtual bool isActive() { return false; }
};

#endif

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_WIRELINK_
#define _YARP2_WIRELINK_

#include <yarp/os/Port.h>
#include <yarp/os/ContactStyle.h>
#include <yarp/os/DummyConnector.h>

namespace yarp {
    namespace os {
        class WireLink;
        class Wire;
    }
}

/**
 *
 * IDL-friendly object state, used in YARP to set up a port association.
 *
 */
class yarp::os::WireLink {
private:
    yarp::os::UnbufferedContactable *port;
    yarp::os::PortReader *reader;
    yarp::os::PortReader *owner;
    bool replies;
    bool can_write;
    bool can_read;

    bool attach(yarp::os::UnbufferedContactable& port, 
                const yarp::os::ContactStyle& style) {
        reset();
        this->port = &port;
        this->replies = style.expectReply;
        return true;
    }

    void reset() {
        reader = 0/*NULL*/; port = 0/*NULL*/;
        replies = true;
        can_write = can_read = true;
    }

public:
    WireLink() { owner = 0/*NULL*/; reset(); }

    /**
     *
     * Check if there is an association of some kind set up for this
     * WireLink.  If not, data has nowhere to go.
     *
     * @return True iff there is an association.
     *
     */
    bool isValid() const { return port!=0/*NULL*/||reader!=0/*NULL*/; }

    /**
     *
     * Set the owner of this WireLink.  Data sent to this link
     * will be passed on to the owner.
     *
     * @param owner owner of this WireLink
     * @return true on success
     *
     */
    bool setOwner(yarp::os::PortReader& owner) {
        this->owner = &owner;
        return true;
    }

    /**
     *
     * Tag this WireLink as a client, sending data via the specified port.
     * @param port the port to use when sending data.
     * @return true on success
     *
     */
    bool attachAsClient(yarp::os::UnbufferedContactable& port) {
        yarp::os::ContactStyle style;
        attach(port,style);
        can_read = false;
        return true;
    }

    /**
     *
     * Tag this WireLink as a client, sending data via the specified reader.
     *
     * @param reader the reader to use when sending data.
     * @return true on success
     *
     */
    bool attachAsClient(yarp::os::PortReader& reader) {
        reset();
        this->reader = &reader;
        can_read = false;
        return true;
    }

    /**
     *
     * Tag this WireLink as a server, receiving commands via the specified port.
     * @param port the port to monitor for commands.
     * @return true on success
     *
     */
    bool attachAsServer(yarp::os::UnbufferedContactable& port) {
        yarp::os::ContactStyle style;
        attach(port,style);
        port.setReader(*owner);
        can_write = false;
        return true;
    }

    /**
     *
     * For a client WireLink, control whether replies to commands are expected.
     * @param streaming true if replies are unnecessary.
     * @return true on success
     *
     */
    bool setStreamingMode(bool streaming) {
        replies = !streaming;
        return true;
    }

    /**
     *
     * Write a message to the associated port or reader.
     * @param writer the message to send.
     * @return true on success
     *
     */
    bool write(PortWriter& writer) {
        if (this->reader) {
            DummyConnector con;
            writer.write(con.getWriter());
            return reader->read(con.getReader());
        }
        if (!isValid()) return false;
        return port->write(writer);
    }

    /**
     *
     * Write a message to the associated port or reader, and read a reply.
     * @param writer the message to send.
     * @param reader a recipient for the reply.
     * @return true on success
     *
     */
    bool write(PortWriter& writer, PortReader& reader) {
        if (this->reader) {
            DummyConnector con;
            writer.write(con.getWriter());
            bool ok = this->reader->read(con.getReader());
            reader.read(con.getReader());
            return ok;
        }
        if (!isValid()) return false;
        if (!replies) { port->write(writer); return false; }
        return port->write(writer,reader);
    }


    bool canWrite() const {
        return can_write;
    }


    bool canRead() const {
        return can_read;
    }
};


#endif

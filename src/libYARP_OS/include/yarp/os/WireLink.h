/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_WIRELINK_H
#define YARP_OS_WIRELINK_H

#include <yarp/os/Port.h>
#include <yarp/os/ContactStyle.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/MessageStack.h>

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
    yarp::os::MessageStack stack;
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
        reader = YARP_NULLPTR;
        port = YARP_NULLPTR;
        replies = true;
        can_write = false;
        can_read = false;
    }

public:
    WireLink() { owner = YARP_NULLPTR; reset(); }

    /**
     *
     * Check if there is an association of some kind set up for this
     * WireLink.  If not, data has nowhere to go.
     *
     * @return True iff there is an association.
     *
     */
    bool isValid() const { return port != YARP_NULLPTR || reader != YARP_NULLPTR; }

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
        can_write = true;
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
        can_write = true;
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
        can_read = true;
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


    /**
     *
     * Put a message in a stack to call later, asynchronously.  Used
     * in implementation of thrift "oneway" messages.
     * @param writer message to send
     * @param reader where to send the message
     * @param tag string to prefix the message with
     * @return true on success
     *
     */
    bool callback(PortWriter& writer, PortReader& reader, const ConstString& tag = "") {
        stack.attach(reader);
        stack.stack(writer,tag);
        return true;
    }

    /**
     *
     * @return true if writing is allowed over link.
     *
     */
    bool canWrite() const {
        return can_write;
    }


    /**
     *
     * @return true if reading from the link is allowed.
     *
     */
    bool canRead() const {
        return can_read;
    }
};


#endif // YARP_OS_WIRELINK_H

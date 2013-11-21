// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PORT_
#define _YARP2_PORT_

#include <yarp/os/api.h>
#include <yarp/os/Portable.h>
#include <yarp/os/UnbufferedContactable.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortWriter.h>
#include <yarp/os/PortReaderCreator.h>

namespace yarp {
    namespace os {
        class Port;
    }
}

/**
 * \ingroup comm_class
 *
 * A mini-server for network communication.
 * It maintains a dynamic collection of incoming and
 * outgoing connections.  Data coming from any incoming connection can
 * be received by calling Port::read.  Calls to Port::write
 * result in data being sent to all the outgoing connections.
 * For help sending and receiving data in the background, see
 * BufferedPort, or PortReaderBuffer and PortWriterBuffer.
 *
 * For examples and help, see: \ref what_is_a_port
 *
 */
class YARP_OS_API yarp::os::Port : public UnbufferedContactable {

public:
    using Contactable::open;

    /**
     * Constructor.  The port begins life in a dormant state. Call
     * Port::open to start things happening.
     */
    Port();


    /**
     * Destructor.
     */
    virtual ~Port();

    // documentation provided in Contactable
    bool open(const ConstString& name);

    // documentation provided in Contactable
    bool open(const Contact& contact, bool registerName = true);

    /**
     *
     * Start port without making it accessible from the network.
     * The port is unusable for many purposes, other than sending
     * out data to specific outputs added with addOutput().
     *
     * @param name an arbitrary name that connections from this
     * port will claim to be from.
     * @return true on success
     *
     */
    bool openFake(const ConstString& name);

    // documentation provided in Contactable
    bool addOutput(const ConstString& name);

    // documentation provided in Contactable
    bool addOutput(const ConstString& name, const ConstString& carrier);

    // documentation provided in Contactable
    bool addOutput(const Contact& contact);

    // documentation provided in Contactable
    void close();

    // documentation provided in Contactable
    virtual void interrupt();

    // documentation provided in Contactable
    virtual void resume();

    // documentation provided in Contactable
    Contact where() const;


    /**
     * Write an object to the port.
     * @param writer any object that knows how to write itself to a
     * network connection - see for example Bottle
     * @param callback object on which to call onCompletion() after write
     * is done (otherwise writer.onCompletion() is called)
     * @return true iff the object is successfully written
     */
    bool write(PortWriter& writer, PortWriter *callback = 0 /*NULL*/) const;

    /**
     * Write an object to the port, then expect one back.
     * @param writer any object that knows how to write itself to a
     * network connection - see for example Bottle
     * @param reader any object that knows how to read itself from a
     * network connection - see for example Bottle
     * @param callback object on which to call onCompletion() after write
     * is done (otherwise writer.onCompletion() is called)
     * @return true iff an object is successfully written and read
     */
    bool write(PortWriter& writer, PortReader& reader,
               PortWriter *callback = 0 /*NULL*/) const;

    /**
     * Read an object from the port.
     * @param reader any object that knows how to read itself from a
     * network connection - see for example Bottle
     * @param willReply you must set this to true if you intend to call reply()
     * @return true iff the object is successfully read
     */
    bool read(PortReader& reader, bool willReply = false);

    /**
     * Send an object as a reply to an object read from the port.
     * Only call this method if you set the willReply flag to
     * true when you called Port::read.
     * @param writer any object that knows how to write itself to a
     * network connection - see for example Bottle
     * @return true iff the object is successfully written
     */
    bool reply(PortWriter& writer);

    /**
     * Same as reply(), but closes connection after reply.
     * This is useful for interoperation with XML/RPC clients
     * that do not expect to reuse a connection.
     *
     * @param writer any object that knows how to write itself to a
     * network connection - see for example Bottle
     * @return true iff the object is successfully written
     */
    bool replyAndDrop(PortWriter& writer);

    // documentation provided in Contactable
    void setReader(PortReader& reader);

    /**
     * Set a creator for readers for port data.
     * Every port that input is received from will be automatically
     * given its own reader.  Handy if you care about the identity
     * of the receiver.
     *
     * @param creator the "factory" for creating PortReader object
     */
    void setReaderCreator(PortReaderCreator& creator);

    /**
     * control whether writing from this port is done in the background.
     * @param backgroundFlag if true, calls to Port::write should return
     * immediately
     */
    void enableBackgroundWrite(bool backgroundFlag);


    // documented in Contactable
    bool isWriting();

    // documented in Contactable
    virtual bool setEnvelope(PortWriter& envelope);

    // documented in Contactable
    virtual bool getEnvelope(PortReader& envelope);

    // documented in Contactable
    virtual int getInputCount();

    // documented in Contactable
    virtual int getOutputCount();

    // documented in Contactable
    virtual void getReport(PortReport& reporter);

    // documented in Contactable
    virtual void setReporter(PortReport& reporter);

    /**
     *
     * Turn on/off "admin" mode.  With admin mode on, messages
     * received from this port will be treated by other ports as
     * administrative messages, rather than as data to pass on to the
     * owner of the port.
     *
     * @param adminMode administration flag
     *
     */
    void setAdminMode(bool adminMode = true);

    /**
     *
     * Configure the port to allow or forbid inputs.  By default,
     * ports allow anything.
     *
     * @param expectInput set to true if this port will be used for input
     *
     */
    void setInputMode(bool expectInput);

    /**
     *
     * Configure the port to allow or forbid outputs.  By default,
     * ports allow anything.
     *
     * @param expectOutput set to true if this port will be used for output
     *
     */
    void setOutputMode(bool expectOutput);

    /**
     *
     * Configure the port to be RPC only.  By default all ports can be
     * used for RPC or streaming communication.
     *
     * @param expectRpc set to true if this port will be used for RPC only
     *
     */
    void setRpcMode(bool expectRpc);

    /**
     *
     * Set a timeout on network operations.  Use a negative number
     * to wait forever (this is the default behavior).  Call this
     * method early, since it won't affect connections that have
     * already been made.
     *
     * @param timeout number of seconds to wait for individual network
     * operations to complete (negative to wait forever).
     *
     */
    bool setTimeout(float timeout);

    /**
     *
     * Set whether the port should issue messages about its operations.
     * @param level verbosity level, -1 to inhibit messages.
     *
     */
    void setVerbosity(int level);

    /**
     *
     * @return port verbosity level.
     *
     */
    int getVerbosity();

    virtual Type getType();
    virtual void promiseType(const Type& typ);

    virtual Property *acquireProperties(bool readOnly);

    virtual void releaseProperties(Property *prop);

private:
    void *implementation;

    bool open(const Contact& contact, bool registerName,
              const char *fakeName);

};

#endif

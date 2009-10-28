// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_PORT_
#define _YARP2_PORT_

#include <yarp/os/Portable.h>
#include <yarp/os/Contactable.h>
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
class yarp::os::Port : public Contactable {

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
    bool open(const char *name);

    // documentation provided in Contactable
    bool open(const Contact& contact, bool registerName = true);

    // documentation provided in Contactable
    bool addOutput(const char *name) {
        return addOutput(Contact::byName(name));
    }

    // documentation provided in Contactable
    bool addOutput(const char *name, const char *carrier) {
        return addOutput(Contact::byName(name).addCarrier(carrier));
    }

    // documentation provided in Contactable
    bool addOutput(const Contact& contact);

    // documentation provided in Contactable
    void close();

    // documentation provided in Contactable
    virtual void interrupt();


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
    bool write(PortWriter& writer, PortWriter *callback = 0 /*NULL*/);

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
     * Send an object as a reply to an object real from the port.
     * Only call this method if you set the willReply flag to 
     * true when you called Port::read.
     * @param writer any object that knows how to write itself to a
     * network connection - see for example Bottle
     * @return true iff the object is successfully written
     */
    bool reply(PortWriter& writer);

    /**
     * Set an external reader for port data.
     * @param reader the external reader to use
     */
    void setReader(PortReader& reader);

    /**
     * Set a creator for readers for port data.
     * Every port that input is received from will be automatically
     * given its own reader.  Handy if you care about the identity
     * of the receiver.
     *
     * @param creator the "factor" for creating PortReader object
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

private:
    void *implementation;

};

#endif

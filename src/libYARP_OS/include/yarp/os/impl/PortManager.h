/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_PORTMANAGER
#define YARP2_PORTMANAGER

#include <yarp/os/ConstString.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/PortReport.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Route.h>

#include <yarp/os/impl/PlatformStdio.h>

namespace yarp {
    namespace os {
        namespace impl {
            class PortManager;
            class PortCoreUnit;
        }
    }
}

/**
 * Specification of minimal operations a port must support in order to
 * be able to make connections.
 */
class YARP_OS_impl_API yarp::os::impl::PortManager {
public:
    /**
     *
     * Constructor.
     *
     */
    PortManager() {
        os = NULL;
        name = "null";
    }

    /**
     *
     * Destructor.
     *
     */
    virtual ~PortManager() {
    }


    /**
     *
     * Set the name of this port.
     * @param name the name of this port
     *
     */
    void setName(const ConstString& name) {
        this->name = name;
    }

    /**
     *
     * Add an output connection to this port.
     * @param dest the name of the target
     * @param id an opaque tracker for the connection
     * @param os the output stream for messages about this operation
     * @param onlyIfNeeded if true, don't add the connection if there
     * is already a connection to the named target
     * @return true on success
     *
     */
    virtual bool addOutput(const ConstString& dest, void *id,
                           yarp::os::OutputStream *os,
                           bool onlyIfNeeded = false) {
        YARP_SPRINTF2(Logger::get(),
                      error,
                      "PortManager for [%s] asked to addOutput [%s]\n",
                      getName().c_str(),
                      dest.c_str());
        return false;
    }


    /**
     *
     * Remove an input connection.
     * @param src the name of the source port
     * @param id an opaque tracker for the connection
     * @param os the output stream for messages about this operation
     *
     */
    virtual void removeInput(const ConstString& src, void *id,
                             yarp::os::OutputStream *os) {
        YARP_SPRINTF2(Logger::get(),
                      error,
                      "PortManager for [%s] asked to removeInput [%s]\n",
                      getName().c_str(),
                      src.c_str());
    }

    /**
     *
     * Remove an output connection.
     * @param dest the name of the target port
     * @param id an opaque tracker for the connection
     * @param os the output stream for messages about this operation
     *
     */
    virtual void removeOutput(const ConstString& dest, void *id,
                              yarp::os::OutputStream *os) {
        YARP_SPRINTF2(Logger::get(),
                      error,
                      "PortManager for [%s] asked to removeOutput [%s]\n",
                      getName().c_str(),
                      dest.c_str());
    }

    /**
     *
     * Remove any connection matching the supplied route.
     * @param route the source/target/carrier associated with the connection
     * @param synch true if we should wait for removal to complete
     * @return true if a connection was found that needed removal
     *
     */
    virtual bool removeIO(const Route& route, bool synch = false) {
        return false;
    }

    /**
     *
     * Produce a text description of the port and its connections.
     * @param id opaque identifier of connection that needs the description
     * @param os stream to write on
     *
     */
    virtual void describe(void *id, yarp::os::OutputStream *os) {
        YARP_SPRINTF1(Logger::get(),
                      error,
                      "PortManager for [%s] asked to describe itself\n",
                      getName().c_str());
    }

    /**
     *
     * Read a block of regular payload data.
     * @param reader source of data
     * @param id opaque identifier of connection providing data
     * @param os stream to write error messages on
     *
     */
    virtual bool readBlock(ConnectionReader& reader, void *id, 
                           yarp::os::OutputStream *os) {
        YARP_SPRINTF1(Logger::get(),
                      error,
                      "PortManager for [%s] asked to deal with data\n",
                      getName().c_str());
        return false;
    }

    /**
     *
     * Read a block of administrative data.
     * @param reader source of data
     * @param id opaque identifier of connection providing data
     * @param os stream to write error messages on
     *
     */
    virtual bool adminBlock(ConnectionReader& reader, void *id, 
                            yarp::os::OutputStream *os) {
        YARP_SPRINTF1(Logger::get(),
                      error,
                      "PortManager for [%s] asked to deal with admin\n",
                      getName().c_str());
        return false;
    }

    /**
     *
     * @return the name of this port
     *
     */
    virtual ConstString getName() {
        return ConstString(name);
    }

    /**
     *
     * Set some envelope information to pass along with a message
     * without actually being part of the message.
     * @param envelope the extra message to send
     *
     */
    virtual void setEnvelope(const ConstString& envelope) = 0;

    /**
     *
     * Handle a port event (connection, disconnection, etc)
     * @param info the event description
     *
     */
    virtual void report(const yarp::os::PortInfo& info) = 0;

    /**
     *
     * Called by a connection handler with active=true just after it
     * is fully configured, and with active=false just before it shuts
     * itself down.
     * @param unit the connection handler starting up / shutting down
     * @param active true if the handler is starting up, false if shutting down
     *
     */
    virtual void reportUnit(PortCoreUnit *unit, bool active) = 0;

protected:
    bool hasOutput() {
        return os!=NULL;
    }

    yarp::os::OutputStream& getOutputStream() {
        yAssert(os!=NULL);
        return *os;
    }

private:
    yarp::os::OutputStream *os;
    ConstString name;
};

#endif


// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARP2_MODULE__
#define __YARP2_MODULE__

#include <yarp/os/IConfig.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Property.h>
#include <yarp/os/all.h>


namespace yarp {
    namespace os {
        class Module;
    }
}

/**
 *
 * A base-class for standard Yarp modules.  If you're building a large
 * system and want to regularize how parts of your system are
 * configured and run, this class may be helpful to you.  Modules are
 * expected to be configurable via IConfig::open() -- in other words,
 * their configuration can be passed in from the command line, from
 * configuration files, the network, or GUIs.  Typically, modules will
 * create a set of ports for communicating with other modules.  We
 * encourage, when possible, that at least one of those ports be
 * hooked up to the respond() method with attach().  For non graphical
 * applications, it is also convenient to hook up standard
 * input/output to the respond() method as well using
 * attachTerminal().
 *
 */
class YARP_OS_API yarp::os::Module : public IConfig {

public:
    /**
     * Constructor.
     */
    Module();

    /**
     * Destructor.
     */
    virtual ~Module();


    /**
     *
     * You can override this to control the approximate periodicity at which
     * updateModule() is called by runModule().  By default, it returns
     * 0 (no delay).
     * @return the desired period between successive calls to updateModule()
     *
     */
    virtual double getPeriod() {
        return 0.0;
    }

    /**
     *
     * Override this to do whatever your module needs to do.  When
     * your module wants to stop, return false.  The module's actual
     * work could be done during this call, or it could just check the
     * state of a thread running in the background.
     * @return true iff module should continue
     *
     */
    virtual bool updateModule() {
        // insert a delay so, if user accidentally doesn't override this
        // method, the thread won't kill the processor
        yarp::os::Time::delay(0.5);
        return true;
    }

    /**
     *
     * Try to halt any ongoing operations by threads managed by the module.
     * By default it does nothing - you may want to override this.
     * If you have created any ports, and have any threads that are
     * might be blocked on reading data from those ports, this is a
     * good place to add calls to BufferedPort::interrupt() or
     * Port::interrupt().  Don't assume this method will always be
     * called on shutdown, or if called will complete before shutdown.
     * @return true if there was no catastrophic failure
     *
     */
    virtual bool interruptModule() {
        return false;
    }


    /**
     *
     * Calls updateModule() until that returns false.
     * updateModule() is called every getPeriod()
     * seconds.  Be aware that the the respond() command could be
     * asycnhronously at any time, if there is input from the
     * standard input or a port connected via attach().
     * @return true on success
     *
     */
    virtual bool runModule();


    /**
     * Simple helper method to call openFromCommand(), then runModule().
     * Most modules that run as a single executable will want to do this.
     * Their main() method would be something like:
     * \code
     *    int main(int argc, char *argv[]) {
     *        Network yarp;
     *        YourModule module;
     *        return module.runModule(argc,argv);
     *    }
     * \endcode
     * @param argc the number of arguments
     * @param argv the list of arguments
     * @param skipFirst set to true if the first argument should be skipped
     * (which is the right thing to do for arguments passed to main())
     * @return 0 upon success, non-zero upon failure
     */
    virtual int runModule(int argc, char *argv[],
                          bool skipFirst = true);


    /**
     * Calls open() with the result of
     * interpreting a list of command line arguments.
     * Also calls attachTerminal().
     * If the command line is "--file filename.ini"
     * it will read the desired configuration from the named file.
     * The configuration file should be or the form needed for
     * yarp::os::Property::fromConfigFile.
     * @param argc the number of arguments
     * @param argv the list of arguments
     * @param skipFirst set to true if the first argument should be skipped
     * (which is the right thing to do for arguments passed to main())
     * @return true/false upon success/failure
     */
    virtual bool openFromCommand(int argc, char *argv[],
                                 bool skipFirst = true);


    /**
     * Respond to a message.  You can override this to respond
     * to messages in your own way.  It is useful, if your module
     * doesn't know what to do with a message, to call
     * Module::respond() for any default responses.
     * @param command the message received
     * @param reply the response you wish to make
     * @return true if there was no critical failure
     */
    virtual bool respond(const Bottle& command, Bottle& reply) {
        return basicRespond(command,reply);
    }

    /**
     * Make any input from a Port object go to the respond() method.
     * @param port the port to attach
     * @return true if port was attached correctly.
     */
    virtual bool attach(Port& port);

    /**
     * Make any input from a BufferedPort or PortReaderBuffer object go to
     * the respond() method.
     * @param port the port or buffer to attach
     * @param handleStream control whether streaming messages (messages that
     * don't expect replies) are also sent to respond().  If they are,
     * replies will be discarded.
     * @return true if port was attached correctly.
     */
    virtual bool attach(TypedReader<Bottle>& port, bool handleStream = false);

    /**
     * Make any input from standard input (usually the keyboard) go to
     * the respond() method.  The reply will be send to standard output.
     * @return true on success.
     */
    virtual bool attachTerminal();

    /**
     * Check if the module is shutting down.
     * @return true if the module is shutting down.
     */
    bool isStopping() { return stopFlag; }


    /**
     * Return name of module, as set with --name flag or setName().
     * @param subName get nested name with this at the end
     * @return the name of the module
     */
    ConstString getName(const char *subName = 0/*NULL*/);

    /**
     * Set the name of the module.
     * @param name the desired name of the module
     */
    void setName(const char *name) {
        this->name = name;
    }

    /**
     * Wrapper around respond() that is guaranteed to process system messages.
     */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    bool safeRespond(const Bottle& command, Bottle& reply);
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

private:
    Property& getState() { return state; }

    virtual bool basicRespond(const Bottle& command, Bottle& reply);

    void *implementation;
    Property state;
    bool stopFlag;
    ConstString name;
};

#endif

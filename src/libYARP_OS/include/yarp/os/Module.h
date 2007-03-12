// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __YARP2_MODULE__
#define __YARP2_MODULE__

#include <yarp/os/IConfig.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Port.h>
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
 * A standard Yarp module.  Modules are expected to accept commands
 * passed to the respond() method.  At a minimum, they should return a
 * short message saying how to find information about them.  The
 * attach() methods configure ports to pass received data to the
 * respond() method.
 *
 */
class yarp::os::Module : public IConfig {

public:
    Module();

    virtual ~Module();


    /**
     *
     * You can control the approximate rate at which updateModule() is
     * called.  Return 0 for no delays.  By default, there are no
     * delays.
     * 
     */
    virtual double getPeriod() {
        return 0.0;
    }

    /**
     *
     * Check if module should continue running.  Return true if so,
     * false if it should stop.  The module's actual work can be done
     * during this call.  Or it can check the state of a thread
     * running in the background.
     * 
     */
    virtual bool updateModule() {
        return true;
    }

    /**
     *
     * Try to halt operations by threads managed by the module.
     * By default it does nothing - you may want to override this.
     *
     */
    virtual bool interruptModule() {
        return false;
    }


    /**
     *
     * Calls updateModule() until that returns false.
     * updateModule() is called every getPeriod()
     * seconds.  The respond() command could be called at any time
     * between open() and close(), if there is input from the 
     * standard input or a port connected via attach().
     *
     */
    virtual bool runModule() {
        while (updateModule()) {
            if (isStopping()) break;
            Time::delay(getPeriod());
            if (isStopping()) break;
        }
        return true;
    }

    /**
     * Respond to a message.  You can override this.
     * @param command the message
     * @param reply the response
     * @return true if there was no critical failure
     */
    virtual bool respond(const Bottle& command, 
                         Bottle& reply);

    /**
     * will read from and reply to port
     */
    virtual bool attach(Port& port);

    /**
     * will read from and reply to port
     */
    virtual bool attach(TypedReader<Bottle>& port);

    /**
     * Read/write from stdin/stdout
     */
    virtual bool attachTerminal();

    Property& getState() { return state; }

    bool isStopping() { return stopFlag; }

private:
    void *implementation;
    Property state;
    bool stopFlag;
};

#endif

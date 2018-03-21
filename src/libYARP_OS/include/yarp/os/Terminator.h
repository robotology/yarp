/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_TERMINATOR_H
#define YARP_OS_TERMINATOR_H


#include <yarp/os/Thread.h>

namespace yarp {
    namespace os {
        class Terminator;
        class Terminee;
    }
}

/**
 * \file Terminator.h classes to handle graceful process termination.
 */

/**
 * Send a message to a process that has a corresponding
 * socket port created using the Terminee class to ask to start
 * a graceful quit procedure. The receiving process must
 * collaborate and take appropriate action after receiving the
 * message. The Terminator/Terminee class pair starts a tcp socket on the
 * server side listening for incoming connections and the magic word "quit"
 * (all lowercase). The ip-port pair is registered with a symbolic name
 * on the Yarp name server and can be queried by the client to send the
 * termination message.
 */
class YARP_OS_API yarp::os::Terminator {
public:
    /**
     * Send a quit message to a specific socket port.
     * @param name is the name of the socket port (as registered in
     * the name server).
     * @return true/false on success/failure.
     */
    static bool terminateByName(const char *name);
};

/**
 * A class that can be polled to see whether the process has been
 * asked to quit gracefully.
 */
class YARP_OS_API yarp::os::Terminee : public yarp::os::Thread {
protected:
    void *implementation;
    volatile bool quit;
    volatile bool ok;

public:
    /**
     * Constructor.
     * @param name is the nickname to register on the name server.
     */
    Terminee(const char *name);

    /**
     * Destructor.
     */
    virtual ~Terminee();


    virtual void run() override;

    /**
     * Call this method to wait for a quit message.
     * @return true if a quit message has been received
     */
    bool waitQuit() const
    {
        // not yet implemented
        return false;
    }

    /**
     * Call this method to see whether a quit message has
     * been received.
     * @return true is a quit has been received, false otherwise.
     */
    bool mustQuit() const
    { return quit; }

    /**
     * Check whether the message mechanism is ok.
     * @return true if ok, false otherwise.
     */
    bool isOk() const { return ok; }

    void onStop() override;
};


#endif // YARP_OS_TERMINATOR_H

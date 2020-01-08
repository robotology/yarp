/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>

#include <iostream>

class MyModule : public yarp::os::RFModule
{
    yarp::os::Port handlerPort; // a port to handle messages
    int count;
public:
    double getPeriod()
    {
        // module periodicity (seconds), called implicitly by the module.
        return 1.0;
    }
    // This is our main function. Will be called periodically every getPeriod() seconds
    bool updateModule()
    {
        count++;
        std::cout << "[" << count << "]" << " updateModule..." << '\n';
        return true;
    }
    // Message handler. Just echo all received messages.
    bool respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
    {
        std::cout << "Got something, echo is on" << '\n';
        if (command.get(0).asString() == "quit")
            return false;
        else
            reply = command;
        return true;
    }
    // Configure function. Receive a previously initialized
    // resource finder object. Use it to configure your module.
    // If you are migrating from the old module, this is the function
    // equivalent to the "open" method.
    bool configure(yarp::os::ResourceFinder &rf)
    {
        count=0;
        if (!handlerPort.open("/myModule"))
            return false;

        // optional, attach a port to the module
        // so that messages received from the port are redirected
        // to the respond method
        attach(handlerPort);

        return true;
    }
    // Interrupt function.
    bool interruptModule()
    {
        std::cout << "Interrupting your module, for port cleanup" << '\n';
        return true;
    }
    // Close function, to perform cleanup.
    bool close()
    {
        // optional, close port explicitly
        std::cout << "Calling close function\n";
        handlerPort.close();
        return true;
    }
};

int main(int argc, char * argv[])
{
    // initialize yarp network
    yarp::os::Network yarp;

    // create your module
    MyModule module;

    // prepare and configure the resource finder
    yarp::os::ResourceFinder rf;
    rf.configure(argc, argv);
    rf.setVerbose(true);

    std::cout << "Configuring and starting module.\n";
    // This calls configure(rf) and, upon success, the module execution begins with a call to updateModule()
    if (!module.runModule(rf)) {
        std::cerr << "Error module did not start\n";
    }

    std::cout << "Main returning..." << '\n';
    return 0;
}

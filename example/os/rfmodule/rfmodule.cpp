/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/LogStream.h>

class MyModule :
        public yarp::os::RFModule
{
    yarp::os::Port handlerPort; // a port to handle messages
    int count;

public:
    double getPeriod() override
    {
        // module periodicity (seconds), called implicitly by the module.
        return 1.0;
    }
    // This is our main function. Will be called periodically every getPeriod() seconds
    bool updateModule() override
    {
        count++;
        yInfo() << "[" << count << "]" << " updateModule...";
        return true;
    }
    // Message handler. Just echo all received messages.
    bool respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply) override
    {
        yInfo() << "Got something, echo is on";
        if (command.get(0).asString() == "quit") {
            return false;
        }
        reply = command;
        return true;
    }
    // Configure function. Receive a previously initialized
    // resource finder object. Use it to configure your module.
    // If you are migrating from the old module, this is the function
    // equivalent to the "open" method.
    bool configure(yarp::os::ResourceFinder& rf) override
    {
        YARP_UNUSED(rf);

        count = 0;
        if (!handlerPort.open("/myModule")) {
            return false;
        }

        // optional, attach a port to the module
        // so that messages received from the port are redirected
        // to the respond method
        attach(handlerPort);

        return true;
    }
    // Interrupt function.
    bool interruptModule() override
    {
        yInfo() << "Interrupting your module, for port cleanup";
        return true;
    }
    // Close function, to perform cleanup.
    bool close() override
    {
        // optional, close port explicitly
        yInfo() << "Calling close function";
        handlerPort.close();
        return true;
    }
};

int main(int argc, char* argv[])
{
    // initialize yarp network
    yarp::os::Network yarp;

    // prepare and configure the resource finder
    yarp::os::ResourceFinder rf;
    rf.configure(argc, argv);

    // create your module
    MyModule module;

    yInfo() << "Configuring and starting module.";
    // This calls configure(rf) and, upon success, the module execution begins with a call to updateModule()
    if (!module.runModule(rf)) {
        yError() << "Error module did not start";
    }

    yInfo() << "Main returning...";
    return 0;
}

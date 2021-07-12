/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>
#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>

using namespace yarp::os;

class MyModule:public RFModule
{
    Port handlerPort; //a port to handle messages
    int count;
public:

    double getPeriod() override
    {
        return 1; //module periodicity (seconds)
    }

    /*
     * This is our main function. Will be called periodically every getPeriod() seconds.
     */
    bool updateModule() override
    {
        count++;
        printf("[%d] updateModule\n", count);

        return true;
    }

    /*
     * Message handler. Just echo all received messages.
     */
    bool respond(const Bottle& command, Bottle& reply) override
    {
        printf("Got something, echo is on\n");
        if (command.get(0).asString()=="quit")
            return false;
        else
            reply=command;
        return true;
    }

    /*
     * Configure function. Receive a previously initialized
     * resource finder object. Use it to configure your module.
     * Open port and attach it to message handler.
     */
    bool configure(yarp::os::ResourceFinder &rf) override
    {
        count=0;
        // handlerPort.open("/regression-test-module");
        // attach(handlerPort);
        // attachTerminal();
        return true;
    }

    /*
     * Interrupt function.
     */
    bool interruptModule() override
    {
        printf("Interrupting your module, for port cleanup\n");
        return true;
    }

    /*
     * Close function, to perform cleanup.
     */
    bool close() override
    {
        printf("Calling close function\n");
        handlerPort.close();
        return true;
    }
};

int main(int argc, char * argv[])
{
    Network yarp;
    MyModule module;

    yarp::os::ResourceFinder rf; //dummy resource finder, empty

    //open a file

    printf("Configure module...\n");
    module.configure(rf);
    printf("Start module...\n");
    module.runModule();

    //remove file
    printf("Main returning...\n");
    return 0;
}

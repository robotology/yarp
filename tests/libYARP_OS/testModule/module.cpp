// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>

using namespace yarp::os;

class MyModule:public RFModule
{
    Port handlerPort; //a port to handle messages
    int count;
public:

    double getPeriod()
    {
        return 1; //module periodicity (seconds)
    }

    /*
    * This is our main function. Will be called periodically every getPeriod() seconds.
    */
    bool updateModule()
    {
        count++;
        printf("[%d] updateModule\n", count);

        return true;
    }

    /*
    * Message handler. Just echo all received messages.
    */
    bool respond(const Bottle& command, Bottle& reply) 
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
    bool configure(yarp::os::ResourceFinder &rf)
    {
        count=0;
      //  handlerPort.open("/regression-test-module");
     //   attach(handlerPort);
     //   attachTerminal();
        return true;
    }

    /*
    * Interrupt function.
    */
    bool interruptModule()
    {
        printf("Interrupting your module, for port cleanup\n");
        return true;
    }

    /*
    * Close function, to perform cleanup.
    */
    bool close()
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



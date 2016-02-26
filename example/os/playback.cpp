/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

/*

This is an example of reading messages without knowing anything about
them.  It makes a port which, if you send messages to it, will be
repeated to anyone reading from that port.

An example of testing it:
  playback --name /test
  yarp write /write /test
  yarp read /read /test

Anything written on the console to the "yarp write" program should end
up being shown on the "yarp read" program, with the "playback" program
showing the number of bytes passed on.

*/

#include <stdio.h>
#include <stdlib.h>
#include <yarp/os/Port.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Stamp.h>
using namespace yarp::os;

/*
  Small class for reading and writing messages.
 */
class Saver : public Portable {
private:
    ManagedBytes mem;
public:
    virtual bool read(ConnectionReader& reader) {
        int len = reader.getSize();
        if (len<=0) {
            fprintf(stderr,"Length of message not provided by carrier.\n");
            return false;
        }
        printf("Reading %d bytes\n", len);
        if (mem.length()<len) {
            mem.allocate(len);
        }
        if (mem.length()!=len) {
            fprintf(stderr,"Could not allocate storage for message.\n");
            return false;
        }
        return reader.expectBlock(mem.get(),mem.length());
    }

    virtual bool write(ConnectionWriter& writer) {
        if (mem.length()==0) {
            fprintf(stderr,"Nothing to write.\n");
            return false;
        }
        writer.appendExternalBlock(mem.get(),mem.length());
        printf("Writing %d bytes\n", mem.length());
        return true;
        
    }
};


int main(int argc, char *argv[]) {
    Network yarp;

    if (argc<2) {
        printf("Call like this:\n");
        printf("  playback --store dirname --name /port/name\n");
        return 0;
    }

    // Get commandline options
    Property options;
    options.fromCommand(argc,argv);
    ConstString portName = 
        options.check("name",Value("/playback"),"port name").asString();

    // Create a port
    Port p;
    p.open(portName.c_str());

    // Loop forever reading messages and writing them again
    while (true) {
        Saver saver;
        p.read(saver);
        p.write(saver);
    }

    return 0;
}


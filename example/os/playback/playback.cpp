/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Property.h>
#include <yarp/os/Value.h>

#include <cstdio>

using yarp::os::ConnectionReader;
using yarp::os::ConnectionWriter;
using yarp::os::ManagedBytes;
using yarp::os::Network;
using yarp::os::Port;
using yarp::os::Portable;
using yarp::os::Property;
using yarp::os::Value;

/*
  Small class for reading and writing messages.
 */
class Saver : public Portable
{
private:
    ManagedBytes mem;

public:
    bool read(ConnectionReader& reader) override
    {
        size_t len = reader.getSize();
        if (len <= 0) {
            fprintf(stderr, "Length of message not provided by carrier.\n");
            return false;
        }
        printf("Reading %zu bytes\n", len);
        if (mem.length() < len) {
            mem.allocate(len);
        }
        if (mem.length() != len) {
            fprintf(stderr, "Could not allocate storage for message.\n");
            return false;
        }
        return reader.expectBlock(mem.get(), mem.length());
    }

    bool write(ConnectionWriter& writer) const override
    {
        if (mem.length() == 0) {
            fprintf(stderr, "Nothing to write.\n");
            return false;
        }
        writer.appendExternalBlock(mem.get(), mem.length());
        printf("Writing %zu bytes\n", mem.length());
        return true;
    }
};


int main(int argc, char* argv[])
{
    Network yarp;

    if (argc < 2) {
        printf("Call like this:\n");
        printf("  playback --store dirname --name /port/name\n");
        return 0;
    }

    // Get commandline options
    Property options;
    options.fromCommand(argc, argv);
    std::string portName = options.check("name", Value("/playback"), "port name").asString();

    // Create a port
    Port p;
    p.open(portName);

    // Loop forever reading messages and writing them again
    while (true) {
        Saver saver;
        p.read(saver);
        p.write(saver);
    }

    return 0;
}

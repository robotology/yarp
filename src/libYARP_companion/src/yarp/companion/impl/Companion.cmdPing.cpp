/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Carriers.h>
#include <yarp/os/Contact.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/OutputProtocol.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/Ping.h>
#include <yarp/os/Route.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/PortCommand.h>
#include <yarp/os/impl/StreamConnectionReader.h>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::Carriers;
using yarp::os::Contact;
using yarp::os::InputStream;
using yarp::os::NetworkBase;
using yarp::os::OutputProtocol;
using yarp::os::OutputStream;
using yarp::os::Ping;
using yarp::os::Route;
using yarp::os::SystemClock;
using yarp::os::impl::BufferedConnectionWriter;
using yarp::os::impl::PortCommand;
using yarp::os::impl::StreamConnectionReader;


int Companion::ping(const char *port, bool quiet)
{

    const char *connectionName = "<ping>";
    OutputProtocol *out = nullptr;

    Contact address = NetworkBase::queryName(port);
    if (!address.isValid()) {
        if (!quiet) {
            yCError(COMPANION, "could not find port");
        }
        return 1;
    }

    if (address.getCarrier()=="tcp") {
        out = Carriers::connect(address);
        if (out == nullptr) {
            yCError(COMPANION, "port found, but cannot connect");
            return 1;
        }
        Route r(connectionName, port, "text_ack");
        bool ok = out->open(r);
        if (!ok) {
            yCError(COMPANION, "could not connect to port");
            return 1;
        }
        OutputStream& os = out->getOutputStream();
        InputStream& is = out->getInputStream();
        StreamConnectionReader reader;

        PortCommand pc(0, "*");
        BufferedConnectionWriter bw(out->getConnection().isTextMode());
        pc.write(bw);
        bw.write(os);
        Bottle resp;
        reader.reset(is, nullptr, r, 0, true);
        bool done = false;
        while (!done) {
            resp.read(reader);
            std::string str = resp.toString();
            if (resp.get(0).asString()!="<ACK>") {
                yCInfo(COMPANION, "%s", str.c_str());
            } else {
                done = true;
            }
        }
        if (out != nullptr) {
            delete out;
        }
    } else {
        int e = NetworkBase::exists(port, quiet);
        yCInfo(COMPANION, "%s %s.", port, (e==0) ? "exists" : "is not responding");
        return e;
    }

    return 0;
}

int Companion::cmdPing(int argc, char *argv[])
{
    bool time = false;
    bool rate = false;
    if (argc>=1) {
        while (argv[0][0]=='-') {
            if (std::string(argv[0])=="--time") {
                time = true;
            } else if (std::string(argv[0])=="--rate") {
                rate = true;
            } else {
                yError("Unrecognized option");
                argc = 1;
            }
            argc--;
            argv++;
        }
    }
    if (argc == 1) {
        char *targetName = argv[0];
        if (time) {
            yCInfo(COMPANION, "Timing communication with %s...", targetName);
            Ping ping;
            ping.setTarget(targetName);
            for (int i=0; i<10; i++) {
                ping.connect();
                ping.report();
                SystemClock::delaySystem(0.25);
            }
            return 0;
        }
        if (rate) {
            yCInfo(COMPANION, "Measuring rate of output from %s...", targetName);
            Ping ping;
            ping.setTarget(targetName);
            ping.sample();
            return 1;
        }
        return ping(targetName, false);
    }
    yCError(COMPANION, "Usage:");
    yCError(COMPANION, "  yarp ping /port");
    yCError(COMPANION, "  yarp ping --time /port");
    yCError(COMPANION, "  yarp ping --rate /port");
    return 1;
}

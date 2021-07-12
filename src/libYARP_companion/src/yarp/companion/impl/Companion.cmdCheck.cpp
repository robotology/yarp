/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/impl/BottleImpl.h>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::NetworkBase;
using yarp::os::Port;
using yarp::os::PortReader;
using yarp::os::SystemClock;
using yarp::os::impl::BottleImpl;

namespace {
class CompanionCheckHelper :
        public yarp::os::PortReader
{
public:
    BottleImpl bot;
    bool got;
    bool read(yarp::os::ConnectionReader& reader) override
    {
        bot.read(reader);
        got = true;
        return true;
    }
    BottleImpl *get() {
        if (got) {
            return &bot;
        }
        return nullptr;
    }
};
} // namespace


int Companion::cmdCheck(int argc, char *argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    yCInfo(COMPANION, "==================================================================");
    yCInfo(COMPANION, "=== This is \"yarp check\"");
    yCInfo(COMPANION, "=== It is a very simple sanity check for your installation");
    yCInfo(COMPANION, "=== If it freezes, try deleting the file reported by \"yarp conf\"");
    yCInfo(COMPANION, "=== Also, if you are mixing terminal types, e.g. bash/cmd.exe");
    yCInfo(COMPANION, "=== on windows, make sure the \"yarp conf\" file is the same on each");
    yCInfo(COMPANION, "==================================================================");
    yCInfo(COMPANION, "=== Trying to register some ports");

    CompanionCheckHelper check;
    Port in;
    bool faking = false;
    if (!NetworkBase::exists(NetworkBase::getNameServerName())) {
        yCInfo(COMPANION, "=== NO NAME SERVER!  Switching to local, fake mode");
        NetworkBase::setLocalMode(true);
        faking = true;
    }
    in.setReader(check);
    in.open("...");
    Port out;
    out.open("...");

    SystemClock::delaySystem(1);

    yCInfo(COMPANION, "==================================================================");
    yCInfo(COMPANION, "=== Trying to connect some ports");

    connect(out.getName().c_str(), in.getName().c_str());

    SystemClock::delaySystem(1);

    yCInfo(COMPANION, "==================================================================");
    yCInfo(COMPANION, "=== Trying to write some data");

    Bottle bot;
    bot.addInt32(42);
    out.write(bot);

    SystemClock::delaySystem(1);

    yCInfo(COMPANION, "==================================================================");
    bool ok = false;
    for (int i=0; i<3; i++) {
        yCInfo(COMPANION, "=== Trying to read some data");
        SystemClock::delaySystem(1);
        if (check.get() != nullptr) {
            int x = check.get()->get(0).asInt32();
            yCInfo(COMPANION, "*** Read number %d", x);
            if (x==42) {
                ok = true;
                break;
            }
        }
    }
    yCInfo(COMPANION, "==================================================================");
    yCInfo(COMPANION, "=== Trying to close some ports");
    in.close();
    out.close();
    SystemClock::delaySystem(1);
    if (!ok) {
        yCInfo(COMPANION, "*** YARP seems broken.");
        //diagnose();
        return 1;
    }

    if (faking) {
        yCInfo(COMPANION, "*** YARP seems okay, but there is no name server available.");
    } else {
        yCInfo(COMPANION, "*** YARP seems okay!");
    }

    return 0;
}

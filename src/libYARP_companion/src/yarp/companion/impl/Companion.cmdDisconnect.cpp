/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::NetworkBase;


/**
 * Request that an output port disconnect from an input port.
 * @param src the name of an output port
 * @param dest the name of an input port
 * @param silent whether to print comments on the result
 * @return 0 on success, non-zero on failure
 */
int Companion::disconnect(const char *src, const char *dest, bool silent)
{
    bool ok = NetworkBase::disconnect(src, dest, silent);
    return ok?0:1;
}


int Companion::unsubscribe(const char *src, const char *dest)
{
    Bottle cmd;
    Bottle reply;
    cmd.addString("unsubscribe");
    cmd.addString(src);
    cmd.addString(dest);
    NetworkBase::write(NetworkBase::getNameServerContact(),
                       cmd,
                       reply);
    bool ok = reply.get(0).toString()=="ok";
    if (!ok) {
        yCInfo(COMPANION, "Unsubscription failed.");
    }
    return ok?0:1;
}



int Companion::cmdDisconnect(int argc, char *argv[])
{
    bool persist = false;
    if (argc>0) {
        if (std::string(argv[0])=="--persist") {
            persist = true;
            argv++;
            argc--;
        }
    }
    if (argc!=2) {
        yCError(COMPANION, "Must have two arguments, a sender port and receiver port");
        return 1;
    }

    const char *src = argv[0];
    const char *dest = argv[1];
    if (persist) {
        return unsubscribe(src, dest);
    }
    return disconnect(src, dest);
}

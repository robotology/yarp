/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>
#include <yarp/companion/impl/BottleReader.h>
#include <yarp/os/ContactStyle.h>
#include <yarp/os/Network.h>

#include <cstring>

using yarp::companion::impl::Companion;
using yarp::companion::impl::BottleReader;
using yarp::os::ContactStyle;
using yarp::os::NetworkBase;


/**
 * Create a port to read Bottles and prints them to standard input.
 * It assumes the Bottles consist of an integer followed by a string.
 * The integer indicates whether the "end-of-file" has been reached.
 * The string is what gets printed.
 * @param name the name which which to register the port
 * @param src name of a port to connect from, if any
 * @param showEnvelope set to true if you want envelope information shown
 * @param trim number of characters of the string that should be printed
 * @return 0 on success, non-zero on failure
 */
int Companion::read(const char *name, const char *src, bool showEnvelope, int trim)
{
    Companion::installHandler();
    BottleReader reader;
    applyArgs(reader.core);
    reader.open(name, showEnvelope, trim);
    if (src != nullptr) {
        ContactStyle style;
        style.quiet = false;
        style.verboseOnSuccess = false;
        NetworkBase::connect(src, reader.getName(), style);
    }
    reader.wait();
    reader.close();
    return 0;
}



int Companion::cmdRead(int argc, char *argv[])
{
    if (argc<1) {
        yCError(COMPANION, "Usage:");
        yCError(COMPANION, "  yarp read <port> [remote port] [envelope] [trim [length]]");
        return 1;
    }

    const char *name = argv[0];
    const char *src = nullptr;
    bool showEnvelope = false;
    size_t trim = -1;
    while (argc>1) {
        if (strcmp(argv[1], "envelope")==0) {
            showEnvelope = true;
        } else if (strcmp(argv[1], "trim") == 0) {
            argc--;
            argv++;
            if (argc > 1) {
                trim = atoi(argv[1]);
            } else {
                static constexpr int default_trim = 80;
                trim = default_trim;
            }
        } else {
            src = argv[1];
        }
        argc--;
        argv++;
    }
    return read(name, src, showEnvelope, trim);
}

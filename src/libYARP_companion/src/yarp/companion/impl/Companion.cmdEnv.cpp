/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/LogStream.h>
#include <yarp/os/SystemClock.h>

#include <yarp/companion/impl/Companion.h>

using yarp::companion::impl::Companion;

extern char** environ;

int Companion::cmdEnv(int argc, char* argv[])
{
    auto cmdEnv_usage = []() {
        yCInfo(COMPANION, "Print the value of environment variables.");
        yCInfo(COMPANION);
        yCInfo(COMPANION, "Usage:");
        yCInfo(COMPANION, "yarp env [variable]");
    };

    if (argc > 1) {
        cmdEnv_usage();
        return EXIT_FAILURE;
    }

    std::string var;
    if (argc == 1) {
        var = argv[0];
        if (var == "--help") {
            cmdEnv_usage();
            return EXIT_SUCCESS;
        }
    }

    char* s = *environ;
    for (int i = 1; s; i++) {
        std::string tmp(s);
        std::string key;
        std::string value;
        size_t equalsSign = tmp.find('=');
        if (equalsSign != std::string::npos) {
            key = tmp.substr(0, equalsSign);
            value = tmp.substr(equalsSign + 1);
        } else {
            yCWarning(COMPANION, "Cannot parse environment variable '%s'", tmp.c_str());
        }

        if (var.empty() || var == key) {
            yCInfo(COMPANION, "%s", s);
            if (!var.empty()) {
                yarp::os::SystemClock::delaySystem(2);
                return EXIT_SUCCESS;
            }
        }

        s = *(environ + i);
    }

    yarp::os::SystemClock::delaySystem(2);

    if (!var.empty()) {
        yCWarning(COMPANION, "Could not find environment variable '%s'", var.c_str());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

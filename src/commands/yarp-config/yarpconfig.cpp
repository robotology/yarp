/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include <yarp/conf/version.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/NameConfig.h>
#include "yarpcontext.h"
#include "yarprobot.h"
#include <cstdio>
#include <cstring>

void show_help() {
    printf("Usage: yarp-config [OPTION]\n\n");
    printf("Known values for OPTION are:\n\n");
    printf("  --help       display this help and exit\n");
    printf("  --version    report version information\n");
    printf("  --namespace  report file that caches the current YARP namespace\n");
    printf("  --nameserver report file that caches nameserver contact information\n");
    printf("\n");
    yarp_context_help();
    yarp_robot_help();
}

int main(int argc, char *argv[]) {
    // Configure system clock
    yarp::os::Network::initMinimum(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::Property options;
    options.fromCommand(argc,argv);
    if (options.check("help")) {
        show_help();
        return 0;
    }
    if (argc>=2) {
        if (!strcmp(argv[1],"context")) {
            return yarp_context_main(argc,argv);
        }
        if (!strcmp(argv[1],"robot")) {
            return yarp_robot_main(argc,argv);
        }
    }
    if (options.check("version")) {
        printf("%s\n", YARP_VERSION);
        return 0;
    }
    if (options.check("namespace")) {
        yarp::os::impl::NameConfig nc;
        printf("%s\n",nc.getConfigFileName(YARP_CONFIG_NAMESPACE_FILENAME).c_str());
        return 0;
    }
    if (options.check("nameserver")) {
        yarp::os::impl::NameConfig nc;
        printf("%s\n",nc.getConfigFileName().c_str());
        return 0;
    }
    show_help();
    return 1;
}

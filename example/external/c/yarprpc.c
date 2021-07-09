/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarpmin.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char buf[1000];
    yarpAddress addr;
    int res;
    if (argc!=3) {
        printf("Call as:\n  %s /port/to/write/to \"message to send\"\n",
               argv[0]);
        exit(1);
    }
    yarp_init();
    res = yarp_port_lookup(&addr,argv[1]);
    if (res<0) {
        printf("Failed to find port\n");
        exit(1);
    }
    fprintf(stderr,"Connecting to %s:%d\n", addr.host, addr.port_number);
    res = yarp_rpc(&addr,argv[2],buf,sizeof(buf));
    if (res<0) {
        printf("RPC failed\n");
        exit(1);
    }
    printf("%s\n", buf);
    yarp_fini();
    return 0;
}

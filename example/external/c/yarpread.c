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
    char *pending_buf = NULL;
    int pending_len = 0;
    yarpAddress addr;
    yarpConnection con;
    int res;
    if (argc<2) {
        printf("Call as:\n  %s /port/to/read/from\n",
               argv[0]);
        exit(1);
    }
    if (argc==4) {
        yarp_init_with(argv[2],atoi(argv[3]));
    } else {
        yarp_init();
    }
    res = yarp_port_lookup(&addr,argv[1]);
    if (res<0) {
        printf("Failed to find port\n");
        exit(1);
    }
    fprintf(stderr,"Connecting to %s:%d\n", addr.host, addr.port_number);
    con = yarp_prepare_to_read(&addr);
    if (!yarp_is_valid(con)) {
        printf("Connection failed\n");
        exit(1);
    }
    while (res>=0) {
        res = yarp_receive_lines(con,buf,sizeof(buf),&pending_buf,&pending_len);
        if (res<=0) break;
        printf("%s\n", buf);
    }
    yarp_disconnect(con);
    yarp_fini();
    return 0;
}

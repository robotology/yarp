// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "yarpmin.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char buf[1000];
    yarpAddress addr;
    yarpConnection con;
    int res;
    if (argc!=2) {
        printf("Call as:\n  %s /port/to/read/from\n",
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
    con = yarp_prepare_to_read(&addr);
    if (!yarp_is_valid(con)) {
        printf("Connection failed\n");
        exit(1);
    }
    while (res>=0) {
        res = yarp_receive_line(con,buf,sizeof(buf));
        if (res>=0) {
            printf("%s\n", buf);
        }
    }
    yarp_disconnect(con);
    yarp_fini();
    return 0;
}

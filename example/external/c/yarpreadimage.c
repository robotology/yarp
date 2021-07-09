/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarpmin.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Buffer to store image.
    // Don't know resolution and format beforehand, so should use
    // conservative numbers.
    unsigned char buf[640*480*4];

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
    con = yarp_prepare_to_read_binary(&addr);
    if (!yarp_is_valid(con)) {
        printf("Connection failed\n");
        exit(1);
    }
    while (res>=0) {
        res = yarp_receive_data_header(con);
        if (res>=0) {
            // get image header, see YARPImagePortContentHeader
            // class in src/libYARP_sig/src/yarp/sig/Image.cpp
            unsigned char header[4*15];
            int image_len = res - sizeof(header);
            int i;
            res = yarp_receive_binary(con,(char*)header,sizeof(header));
            if (res<0) {
                printf("Failed to read image header\n");
                exit(1);
            }
            char format[5] = {0,0,0,0,0};
            for (i=0; i<4; i++) {
                format[i] = header[4*5+i];
            }
            int depth = yarp_read_int(header+4*8,4); // header.depth
            int width = yarp_read_int(header+4*11,4); // header.width
            int height = yarp_read_int(header+4*12,4); // header.height
            printf("Received image, size %dx%d, pixel depth %d, format %s\n",
                   width, height, depth, format);
            if (image_len!=width*height*depth) {
                printf("Image may have padding, yarpreadimage.c needs to be updated to deal with that.\n");
                exit(1);
            }
            if (image_len>sizeof(buf)) {
                printf("Image too big to store, increase buffer size...\n");
                exit(1);
            }
            res = yarp_receive_binary(con,(char*)buf,image_len);
            // process the buffer as you wish...
        }
    }
    yarp_disconnect(con);
    yarp_fini();
    return 0;
}

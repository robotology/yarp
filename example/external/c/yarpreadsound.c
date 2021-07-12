/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarpmin.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Buffer to store sound.
    // Make sure this is big enough for your case, or change it to be
    // allocated dynamically.
    unsigned char buf[16384*2*4];

    // Buffer to store sound properties
    unsigned char properties[1000];

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
        int total =  yarp_receive_data_header(con);
        if (total>=0) {
            // YARP sound format is just like images (see yarpreadimage)
            // EXCEPT:
            //   There is an extra header, declaring the image and a
            //   small extra Bottle holding sound details like the
            //   sampling frequency.

            unsigned char preheader[4*2]; // declares header and footer
            unsigned char header[4*15];   // image format
            unsigned char footer[4*3];    // bottle format

            res = yarp_receive_binary(con,(char*)preheader,8);
            if (res<0) {
                printf("Failed to read sound initial header\n");
                exit(1);
            }

            // get image header, see YARPImagePortContentHeader
            // class in src/libYARP_sig/src/yarp/sig/Image.cpp
            int i;
            res = yarp_receive_binary(con,(char*)header,sizeof(header));
            if (res<0) {
                printf("Failed to read sound 'image' header\n");
                exit(1);
            }
            char format[5] = {0,0,0,0,0};
            for (i=0; i<4; i++) {
                format[i] = header[4*5+i];
            }
            int depth = yarp_read_int(header+4*8,4); // header.depth
            int width = yarp_read_int(header+4*11,4); // header.width
            int height = yarp_read_int(header+4*12,4); // header.height

            int image_len = total-sizeof(preheader)-sizeof(header)-
                sizeof(footer);
            if (image_len>sizeof(buf)) {
                printf("Image too big to store, increase buffer size...\n");
                exit(1);
            }
            res = yarp_receive_binary(con,(char*)buf,image_len);
            // process the buffer as you wish...

            res = yarp_receive_binary(con,(char*)footer,sizeof(footer));
            if (res<0) {
                printf("Failed to read sound footer\n");
                exit(1);
            }
            int freq = yarp_read_int(footer+4*2,4);
            printf("Received sound, size %dx%d, sample size %d, format %s, frequency %d\n",
                   width, height, depth, format, freq);

            if (image_len!=width*height*depth) {
                printf("Sound may have padding, yarpreadsound.c needs to be updated to deal with that (or use \n");
                exit(1);
            }
        }
    }
    yarp_disconnect(con);
    yarp_fini();
    return 0;
}

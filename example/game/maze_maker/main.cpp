/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <stdio.h>

#include <fstream>

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;

using namespace std;

int main(int argc, char *argv[]) {
    if (argc==1) {
        printf("Please supply source image filename (e.g. \"in.ppm\")\n");
        printf("Then I will make maze.txt and maze.ppm\n");
        exit(1);
    }
    ImageOf<PixelRgb> img;
    if (!read(img,argv[1])) {
        printf("Failed to read image %s\n",argv[1]);
        exit(1);
    }
    printf("Read a %dx%d image\n", img.width(), img.height());

    printf("Converting to gray scale\n");
    ImageOf<PixelMono> mono;
    mono.copy(img);

    printf("Thresholding\n");
    IMGFOR(mono,x,y) {
        mono.pixel(x,y) = 255*(mono.pixel(x,y)>=128);
    }

    img.copy(mono);

    printf("Writing maze.ppm\n");
    write(img,"maze.ppm");


    printf("Writing maze.txt\n");
    ofstream fout("maze.txt");
    if (0) {
        for (int x=0; x<mono.width(); x++) {
            for (int y=0; y<mono.height(); y++) {
                if (mono(x,y)<128) {
                    fout << "#";
                } else {
                    fout << " ";
                }
            }
            fout << endl;
        }
    } else {
        for (int y=0; y<mono.height(); y++) {
            for (int x=0; x<mono.width(); x++) {
                if (mono(x,y)<128) {
                    fout << "#";
                } else {
                    fout << " ";
                }
            }
            fout << endl;
        }
    }
    fout.close();

    return 0;
}

/*
 * Copyright (C) 2007 Giacomo Spigler
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <stdlib.h>
#include <yarp/dev/GPUInterface.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>
#include <yarp/sig/all.h>
#include <yarp/os/all.h>

#include <cv.h>
#include <cvaux.h>
#include <highgui.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;
using namespace yarp::dev;

int main(int argc, char *argv[]) {
    //Instantiate the GPU driver
    ImageOf<PixelRgb> img;
    yarp::sig::file::read(img, "../dev/image/img0250.ppm");

    ImageOf<PixelRgb> out=img;


    // use YARP to create and configure an instance of NVIDIAGPU
    Property config;
    // no arguments, use a default
    char str[80];
    sprintf(str, "(device nvidia) (w %d) (h %d) (bpp 3)", img.width(), img.height());
    config.fromString(str);

    PolyDriver dd(config);
    if (!dd.isValid()) {
        printf("Failed to create and configure a device\n");
        exit(1);
    }

    IGPUDevice *gpu;
    if (!dd.view(gpu)) {
        printf("Failed to view device through IGPUDevice interface\n");
        exit(1);
    }


    //Load and execute a program for the GPU
    int prog;
    if(argc!=2) {
        prog = gpu->load("progs/FP-BW.cg");
    } else {
        prog = gpu->load(argv[1]);
    }
    write(out,"test0.ppm");
    gpu->execute(prog, &img, &out);
    write(out,"test1.ppm");

    //Show the resulting image onto the screen using OpenCV
    IplImage *cvImage = cvCreateImage(cvSize(out.width(), out.height()),  IPL_DEPTH_8U, 3);
    cvCvtColor((IplImage*)out.getIplImage(), cvImage, CV_RGB2BGR);

    cvNamedWindow("NVIDIA", 1);
    cvShowImage("NVIDIA", cvImage);

    cvWaitKey(3000);
    cvDestroyWindow("NVIDIA");

    cvReleaseImage(&cvImage);

    dd.close();
    return 0;
}




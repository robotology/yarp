/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>

#include <cv.h>
#include <cvaux.h>
#include <highgui.h>

#include <yarp/sig/all.h>

using namespace yarp::sig;
using namespace yarp::sig::draw;
using namespace yarp::sig::file;


int main(int argc, char *argv[]) {
    printf("Show a circle for 3 seconds...\n");
    ImageOf<PixelRgb> yarpImage;


    printf("Creating a YARP image of a nice circle\n");
    yarpImage.resize(300,200);
    addCircle(yarpImage,PixelRgb(255,0,0),
              yarpImage.width()/2,yarpImage.height()/2,
              yarpImage.height()/4);
    addCircle(yarpImage,PixelRgb(255,50,50),
              yarpImage.width()/2,yarpImage.height()/2,
              yarpImage.height()/5);


    printf("Copying YARP image to an OpenCV/IPL image\n");
    IplImage *cvImage = cvCreateImage(cvSize(yarpImage.width(),  
                                             yarpImage.height()), 
                                      IPL_DEPTH_8U, 3 );
    cvCvtColor((IplImage*)yarpImage.getIplImage(), cvImage, CV_RGB2BGR);

    printf("Showing OpenCV/IPL image\n");
    cvNamedWindow("test",1);
    cvShowImage("test",cvImage);

    printf("Taking image back into YARP...\n");
    ImageOf<PixelBgr> yarpReturnImage;
    yarpReturnImage.wrapIplImage(cvImage);
    yarp::sig::file::write(yarpReturnImage,"test.ppm");
    printf("Saving YARP image to test.ppm\n");


    cvWaitKey(3000);

    cvDestroyWindow("test");

    cvReleaseImage(&cvImage);

    printf("...done\n");
    return 0;
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageDraw.h>
#include <yarp/sig/ImageFile.h>
#include <yarp/cv/Cv.h>


int main(int argc, char *argv[]) {
    std::cout<<"Show a circle for 3 seconds..."<<std::endl;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> yarpImage;

    std::cout<<"Creating a YARP image of a nice circle"<<std::endl;
    yarpImage.resize(300,200);
    yarpImage.zero();
    yarp::sig::draw::addCircle(yarpImage,yarp::sig::PixelRgb(255,0,0),
                               yarpImage.width()/2,yarpImage.height()/2,
                               yarpImage.height()/4);
    yarp::sig::draw::addCircle(yarpImage,yarp::sig::PixelRgb(255,50,50),
                               yarpImage.width()/2,yarpImage.height()/2,
                               yarpImage.height()/5);

    yarp::sig::file::write(yarpImage,"test-1.ppm");
    std::cout<<"Saving YARP image to test-1.ppm"<<std::endl;

    std::cout<<"Showing OpenCV image"<<std::endl;
    cv::Mat cvImage1=yarp::cv::toCvMat(yarpImage);
    cv::namedWindow("test",1);
    cv::imshow("test",cvImage1);

    std::cout<<"Taking image back into YARP..."<<std::endl;
    cv::Mat cvImage2=cvImage1.clone();
    auto yarpReturnImage=yarp::cv::fromCvMat<yarp::sig::PixelRgb>(cvImage2);
    yarp::sig::file::write(yarpReturnImage,"test-2.ppm");
    std::cout<<"Saving YARP image to test-2.ppm"<<std::endl;

    cv::waitKey(3000);
    cv::destroyWindow("test");

    std::cout<<"...done"<<std::endl;
    return 0;
}

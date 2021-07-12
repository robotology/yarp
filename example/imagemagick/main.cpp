/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <Magick++.h>
#include <iostream>

#include <yarp/sig/Image.h>

using namespace std;

void copyImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& src,
               Magick::Image& dest) {
    int h = src.height();
    int w = src.width();
    dest.size(Magick::Geometry(w,h));
    dest.depth(8);
    for (int i=0; i<h; i++) {
        // must transfer row by row, since YARP may use padding in representation
        Magick::PixelPacket *packet = dest.setPixels(0,i,w,1);
        dest.readPixels(Magick::RGBQuantum,(unsigned char *)(&src.pixel(0,i)));
    }
    dest.syncPixels();
}


void copyImage(Magick::Image& src,
               yarp::sig::ImageOf<yarp::sig::PixelRgb>& dest) {
    Magick::Geometry g = src.size();
    int h = g.height();
    int w = g.width();
    src.depth(8);
    dest.resize(w,h);
    for (int i=0; i<h; i++) {
        // must transfer row by row, since YARP may use padding in representation
        Magick::PixelPacket *packet = src.getPixels(0,i,w,1);
        src.writePixels(Magick::RGBQuantum,(unsigned char *)(&dest.pixel(0,i)));
    }
    src.syncPixels();
}


int main(int argc,char **argv)
{
    try {
        yarp::sig::ImageOf<yarp::sig::PixelRgb> yimg1, yimg2;

        yimg1.resize(255,127);
        for (int i=0; i<yimg1.width(); i++) {
            for (int j=0; j<yimg1.height(); j++) {
                yarp::sig::PixelRgb& pix = yimg1.pixel(i,j);
                pix.r = ((i+j)/2)%256;
                pix.g = i%256;
                pix.b = j%256;
            }
        }

        printf("Creating a YARP image, and showing the value of one pixel\n");

        yarp::sig::PixelRgb& pixel1 = yimg1.pixel(10,20);
        printf("rgb %d %d %d\n", pixel1.r, pixel1.g, pixel1.b);

        Magick::Image mimg;
        copyImage(yimg1,mimg);

        printf("Copying image to Magick, and tracking the value of the same pixel\n");

        Magick::Color c = mimg.pixelColor(10, 20);
        printf("rgb %d %d %d\n", c.redQuantum(),c.greenQuantum(),c.blueQuantum());

        printf("Saving image as test.gif\n");
        mimg.write("test.gif");

        copyImage(mimg,yimg2);

        printf("Copying image back to YARP, and tracking the value of the same pixel\n");

        yarp::sig::PixelRgb& pixel2 = yimg2.pixel(10,20);
        printf("rgb %d %d %d\n", pixel2.r, pixel2.g, pixel2.b);
    }
    catch( Magick::Exception &error_ )
        {
            cout << "Caught exception: " << error_.what() << endl;
            return 1;
        }
    return 0;
}

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/dev/TestFrameGrabber.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/sig/ImageDraw.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::sig::draw;

#define VOCAB_BALL VOCAB4('b','a','l','l')


void TestFrameGrabber::createTestImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>&
                                       image) {
    image.resize(w,h);
    image.zero();
    switch (mode) {
    case VOCAB_BALL:
        {
            addCircle(image,PixelRgb(0,255,0),bx,by,15);
            addCircle(image,PixelRgb(0,255,255),bx,by,8);
            if (ct%5!=0) {
                rnd *= 65537;
                rnd += 17;
                bx += (rnd%5)-2;
                rnd *= 65537;
                rnd += 17;
                by += (rnd%5)-2;
            } else {
                int dx = w/2 - bx;
                int dy = h/2 - by;
                if (dx>0) { bx++; }
                if (dx<0) { bx--; }
                if (dy>0) { by++; }
                if (dy<0) { by--; }
            }
        }
        break;
    case VOCAB_LINE:
    default:
        {
            for (int i=0; i<image.width(); i++) {
                image.pixel(i,ct).r = 255;
            }
        }
        break;
    }
    ct++;
    if (ct>=image.height()) {
        ct = 0;
    }
    if (by>=image.height()) {
        by = image.height()-1;
    }
    if (bx>=image.width()) {
        bx = image.width()-1;
    }
    if (bx<0) bx = 0;
    if (by<0) by = 0;
}


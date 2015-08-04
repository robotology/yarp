// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/dev/TestFrameGrabber.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/sig/ImageDraw.h>
#include <yarp/os/Random.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::sig::draw;

#define VOCAB_BALL VOCAB4('b','a','l','l')
#define VOCAB_GRID VOCAB4('g','r','i','d')
#define VOCAB_RAND VOCAB4('r','a','n','d')
#define VOCAB_NONE VOCAB4('n','o','n','e')


void TestFrameGrabber::createTestImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>&
                                       image) {
    // to test IPreciselyTimed, make timestamps be mysteriously NNN.NNN42
    double t = Time::now();
    t -= ((t*1000)-(int)t)/1000;
    t+= 0.00042;
    stamp.update(t);
    if (background.width()>0) {
        image.copy(background);
    } else {
        image.zero();
        image.resize(w,h);
    }
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
    case VOCAB_GRID:
        {
            int ww = image.width();
            int hh = image.height();
            if (ww>1&&hh>1) {
                for (int x=0; x<ww; x++) {
                    for (int y=0; y<hh; y++) {
                        double xx = ((double)x)/(ww-1);
                        double yy = ((double)y)/(hh-1);
                        int r = int(0.5+255*xx);
                        int g = int(0.5+255*yy);
                        bool act = (y==ct);
                        image.pixel(x,y) = PixelRgb(r,g,act*255);
                    }
                }
            }
        }
        break;
    case VOCAB_LINE:
    default:
        {
            for (int i=0; i<image.width(); i++) {
                image.pixel(i,ct).r = 255;
            }
            char ttxt[50];
            sprintf(ttxt, "%10.10f", t);
            image.pixel(0, 0).r = ttxt[0]-'0';
            image.pixel(0, 0).g = ttxt[1] - '0';
            image.pixel(0, 0).b = ttxt[2] - '0';

            image.pixel(1, 0).r = ttxt[3] - '0';
            image.pixel(1, 0).g = ttxt[4] - '0';
            image.pixel(1, 0).b = ttxt[5] - '0';

            image.pixel(2, 0).r = ttxt[6] - '0';
            image.pixel(2, 0).g = ttxt[7] - '0';
            image.pixel(2, 0).b = ttxt[8] - '0';

            image.pixel(3, 0).r = ttxt[9]; -'0';
            image.pixel(3, 0).g = ttxt[10] - '0';
            image.pixel(3, 0).b = ttxt[11] - '0';

            image.pixel(4, 0).r = ttxt[12] - '0';
            image.pixel(4, 0).g = ttxt[13] - '0';
            image.pixel(4, 0).b = ttxt[14] - '0';

            image.pixel(5, 0).r = ttxt[15] - '0';
            image.pixel(5, 0).g = ttxt[16] - '0';
            image.pixel(5, 0).b = ttxt[17] - '0';

            image.pixel(6, 0).r = ttxt[18] - '0';
            image.pixel(6, 0).g = ttxt[19] - '0';
            image.pixel(6, 0).b = ttxt[20] - '0';
        }
        break;
    case VOCAB_RAND:
        {
            // from Alessandro Scalzo

            static unsigned char r=128,g=128,b=128;

            int ww = image.width();
            int hh = image.height();

            if (ww>1&&hh>1) {
                for (int x=0; x<ww; x++) {
                    for (int y=0; y<hh; y++) {
                        //r+=(rand()%3)-1;
                        //g+=(rand()%3)-1;
                        //b+=(rand()%3)-1;
                        r += Random::uniform(-1,1);
                        g += Random::uniform(-1,1);
                        b += Random::uniform(-1,1);
                        image.pixel(x,y) = PixelRgb(r,g,b);
                    }
                }
            }
        }
        break;
    case VOCAB_NONE:
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



// From iCub staticgrabber device.
// DF2 bayer sequence.
// -- in staticgrabber: first row GBGBGB, second row RGRGRG.
// -- changed here to:  first row GRGRGR, second row BGBGBG.
bool TestFrameGrabber::makeSimpleBayer(
        ImageOf<PixelRgb>& img,
        ImageOf<PixelMono>& bayer) {

    bayer.resize(img.width(), img.height());

    const int w = img.width();
    const int h = img.height();

    int i, j;
    for (i = 0; i < h; i++) {
        PixelRgb *row = (PixelRgb *)img.getRow(i);
        PixelMono *rd = (PixelMono *)bayer.getRow(i);

        for (j = 0; j < w; j++) {

            if ((i%2) == 0) {
                switch (j%4) {
                    case 0:
                    case 2:
                        *rd++ = row->g;
                        row++;
                        break;

                    case 1:
                    case 3:
                        *rd++ = row->r;
                        row++;
                        break;
                }
            }

            if ((i%2) == 1) {
                switch (j%4) {
                    case 1:
                    case 3:
                        *rd++ = row->g;
                        row++;
                        break;

                    case 0:
                    case 2:
                        *rd++ = row->b;
                        row++;
                        break;
                }
            }
        }
    }

    return true;
}

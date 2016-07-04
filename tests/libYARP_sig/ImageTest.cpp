/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


/**
 *
 * Tests for images
 *
 */

#include <yarp/os/NetType.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageDraw.h>
#include <yarp/os/Network.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>

#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::sig::draw;
using namespace yarp::os;

class ImageTest : public UnitTest {
public:
    virtual ConstString getName() { return "ImageTest"; }

    void passImage(ImageOf<PixelRgb> img) {
        report(0, "passed a blank image ok");
    }

    void testCreate() {
        report(0,"testing image creation...");
        FlexImage image;
        image.setPixelCode(VOCAB_PIXEL_RGB);
        image.resize(256,128);
        checkEqual(image.width(),256,"check width");
        checkEqual(image.height(),128,"check height");
        ImageOf<PixelInt> iint;
        iint.resize(256,128);
        long int total = 0;
        for (int x=0; x<iint.width(); x++) {
            for (int y=0; y<iint.height(); y++) {
                int v = (x+y)%65537;
                iint.pixel(x,y) = v;
                total += v;
            }
        }
        for (int x2=0; x2<iint.width(); x2++) {
            for (int y2=0; y2<iint.height(); y2++) {
                total -= iint.pixel(x2,y2);
            }
        }
        checkEqual(total,0,"pixel assignment check");
    }

    void testCopy() {
        report(0,"testing image copying...");

        ImageOf<PixelRgb> img1;
        img1.resize(128,64);
        for (int x=0; x<img1.width(); x++) {
            for (int y=0; y<img1.height(); y++) {
                PixelRgb& pixel = img1.pixel(x,y);
                pixel.r = x;
                pixel.g = y;
                pixel.b = 42;
            }
        }

        ImageOf<PixelRgb> result;
        result.copy(img1);

        checkEqual(img1.width(),result.width(),"width check");
        checkEqual(img1.height(),result.height(),"height check");
        if (img1.width()==result.width() &&
            img1.height()==result.height()) {
            int mismatch = 0;
            for (int x=0; x<img1.width(); x++) {
                for (int y=0; y<img1.height(); y++) {
                    PixelRgb& pix0 = img1.pixel(x,y);
                    PixelRgb& pix1 = result.pixel(x,y);
                    if (pix0.r!=pix1.r ||
                        pix0.g!=pix1.g ||
                        pix0.b!=pix1.b) {
                        mismatch++;
                    }
                }
            }
            checkTrue(mismatch==0,"pixel match check");
        }
    }

    void testZero() {
        report(0,"testing image zeroing...");
        ImageOf<PixelRgb> img1;
        img1.resize(128,64);
        img1.pixel(20,10).r = 42;
        checkEqual(img1.pixel(20,10).r,42,"pixel set");
        img1.zero();
        checkEqual(img1.pixel(20,10).r,0,"pixel unset");
    }

    void testCast() {
        report(0,"testing image casting...");

        ImageOf<PixelRgb> img1;
        img1.resize(128,64);
        for (int x=0; x<img1.width(); x++) {
            for (int y=0; y<img1.height(); y++) {
                PixelRgb& pixel = img1.pixel(x,y);
                unsigned char v = x%30;
                pixel.r = v;
                pixel.g = v;
                pixel.b = v;
            }
        }

        ImageOf<PixelMono> result;
        result.copy(img1);

        checkEqual(img1.width(),result.width(),"width check");
        checkEqual(img1.height(),result.height(),"height check");

        if (img1.width()==result.width() &&
            img1.height()==result.height()) {
            int mismatch = 0;
            for (int x=0; x<img1.width(); x++) {
                for (int y=0; y<img1.height(); y++) {
                    PixelRgb& pix0 = img1.pixel(x,y);
                    PixelMono& pix1 = result.pixel(x,y);
                    if (pix0.r>pix1+1 || pix0.r<pix1-1) {
                        mismatch++;
                    }
                }
            }
            checkTrue(mismatch==0,"pixel match check");
        }
    }


#define EXT_WIDTH (128)
#define EXT_HEIGHT (64)

    void testExternal() {
        report(0, "testing external image...");
        unsigned char buf[EXT_HEIGHT][EXT_WIDTH];

        {
            for (int x=0; x<EXT_WIDTH; x++) {
                for (int y=0; y<EXT_HEIGHT; y++) {
                    buf[y][x] = 20;
                }
            }
        }

        ImageOf<PixelMono> img1;

        img1.setExternal(&buf[0][0],EXT_WIDTH,EXT_HEIGHT);

        checkEqual(img1.width(),EXT_WIDTH,"width check");
        checkEqual(img1.height(),EXT_HEIGHT,"height check");

        int mismatch = 0;
        for (int x=0; x<img1.width(); x++) {
            for (int y=0; y<img1.height(); y++) {
                img1.pixel(x,y) = 5;
                if (buf[y][x]!=5) {
                    mismatch++;
                }
            }
        }
        checkEqual(mismatch,0,"delta check");

        report(0, "testing various padding + alignments...");
        for (int ww=1; ww<=17; ww++) {
            for (int hh=1; hh<=17; hh++) {
                for (int pad1=1; pad1<=9; pad1++) {
                    for (int pad2=1; pad2<=9; pad2++) {
                        int wwp1 = (ww%pad1)?(ww+pad1-(ww%pad1)):ww;
                        FlexImage img;
                        char *data = new char[wwp1*hh*3];
                        yAssert(data);
                        img.setQuantum(pad1);
                        img.setPixelCode(VOCAB_PIXEL_RGB);
                        img.setPixelSize(3);
                        img.setExternal(data,ww,hh);
                        ImageOf<PixelRgb> target;
                        target.setQuantum(pad2);
                        target.copy(img);
                        delete[] data;
                    }
                }
            }
        }
    }


    void testTransmit() {
        report(0,"testing image transmission...");

        ImageOf<PixelRgb> img1;
        img1.resize(128,64);
        for (int x=0; x<img1.width(); x++) {
            for (int y=0; y<img1.height(); y++) {
                PixelRgb& pixel = img1.pixel(x,y);
                pixel.r = x;
                pixel.g = y;
                pixel.b = 42;
            }
        }

        PortReaderBuffer< ImageOf<PixelRgb> > buf;

        Port input, output;

        buf.setStrict();
        buf.attach(input);

        input.open("/in");
        output.open("/out");

        output.addOutput(Contact::byName("/in").addCarrier("tcp"));
        Time::delay(0.2);

        report(0,"writing...");
        output.write(img1);
        output.write(img1);
        output.write(img1);
        report(0,"reading...");
        ImageOf<PixelRgb> *result = buf.read();

        checkTrue(result!=NULL,"got something check");
        if (result!=NULL) {
            checkEqual(img1.width(),result->width(),"width check");
            checkEqual(img1.height(),result->height(),"height check");
            if (img1.width()==result->width() &&
                img1.height()==result->height()) {
                int mismatch = 0;
                for (int x=0; x<img1.width(); x++) {
                    for (int y=0; y<img1.height(); y++) {
                        PixelRgb& pix0 = img1.pixel(x,y);
                        PixelRgb& pix1 = result->pixel(x,y);
                        if (pix0.r!=pix1.r ||
                            pix0.g!=pix1.g ||
                            pix0.b!=pix1.b) {
                            mismatch++;
                        }
                    }
                }
                checkTrue(mismatch==0,"pixel match check");
            }
        }

        output.close();
        input.close();
    }


    void testPadding() {
        report(0,"checking image padding...");
        ImageOf<PixelMono> img1;
        img1.resize(13,5);
        checkEqual(img1.getQuantum(),8,"ipl compatible quantum");
        checkEqual(img1.getRowSize(),16,"ipl compatible row size");
        checkEqual(img1.width(),13,"good real row width");
        checkEqual(img1.getPadding(), img1.getRowSize()-img1.width(), "getPadding()");

        unsigned char buf2[13][5];
        ImageOf<PixelMono> img2;
        img2.setExternal(&buf2[0][0],13,5);
        checkEqual(img2.getQuantum(),1,"natural external quantum");
        checkEqual(img2.getRowSize(),13,"natural external row size");
        checkEqual(img2.width(),13,"natural external row width");
        checkEqual(img2.getPadding(), 0, "natural external padding");

        unsigned char buf3[16][5];
        ImageOf<PixelMono> img3;
        img3.setQuantum(8);
        img3.setExternal(&buf3[0][0],13,5);
        checkEqual(img3.getQuantum(),8,"forced external quantum");
        checkEqual(img3.getRowSize(),16,"forced external row size");
        checkEqual(img3.width(),13,"normal external row width");
        checkEqual(img3.getPadding(), 3, "forced external padding");

        FlexImage img4;
        img4.setPixelCode(VOCAB_PIXEL_MONO);
        img4.setQuantum(1);
        img4.resize(10,10);
        checkEqual(img4.getQuantum(),1,"unit quantum");
        checkEqual(img4.getRowSize(),10,"exact row size");
    }

    void testStandard() {
        report(0,"checking standard compliance of description...");
        ImageOf<PixelRgb> img;
        img.resize(8,4);
        img.zero();
        BufferedConnectionWriter writer;
        img.write(writer);
        ConstString s = writer.toString();
        Bottle bot;
        bot.fromBinary(s.c_str(),s.length());
        checkEqual(bot.size(),4,"plausible bottle out");
        checkEqual(bot.get(0).toString().c_str(),"mat","good tag");
        YARP_DEBUG(Logger::get(),"an example image:");
        YARP_DEBUG(Logger::get(),bot.toString().c_str());
    }

    void testDraw() {
        report(0,"checking draw tools...");
        ImageOf<PixelRgb> img;
        img.resize(64,64);
        img.zero();
        addCircle(img,PixelRgb(255,0,0),32,32,200);
        // full image should be colored blue
        bool ok = true;
        IMGFOR(img,x,y) {
            if (img.pixel(x,y).r!=255) {
                ok = false;
            }
        }
        checkTrue(ok,"image is blue");
    }


    void testScale() {
        report(0,"checking scaling...");
        ImageOf<PixelRgb> img;
        ImageOf<PixelMono> img2;
        ImageOf<PixelRgb> img3;
        img.resize(64,64);
        img.zero();
        for (int i=0; i<img.width()/2; i++) {
            for (int j=0; j<img.height()/2; j++) {
                img(i,j).r = 255;
                img(i,j).g = 255;
                img(i,j).b = 255;
            }
        }
        img2.copy(img,32,32);
        checkEqual(img2.width(),32,"dimension check");
        checkEqual(img2(0,0),255,"logic check");
        checkEqual(img2(img2.width()-2,0),0,"logic check");
        checkEqual(img2(0,img2.height()-2),0,"logic check");
        img3.copy(img,16,16);
        checkEqual(img3.width(),16,"dimension check");
        checkEqual(img3(0,0).r,255,"logic check");
        checkEqual(img3(img3.width()-2,0).r,0,"logic check");
        checkEqual(img3(0,img3.height()-2).r,0,"logic check");
        img.copy(img3,4,4);
        checkEqual(img.width(),4,"dimension check");
    }

    // test row pointer access (getRow())
    // this function only tests if getRow(r)[c] is consistent with the operator ()
    void testRowPointer()
    {
        report(0,"checking row pointer...");
        ImageOf<PixelRgb> img1;
        ImageOf<PixelRgb> img2;

        img1.resize(59, 50);
        img2.resize(59, 50);

        img1.zero();
        img2.zero();

        int r,c;
        // fill img1 with some data, using pointer to row
        // access
        for(r=0; r<img1.height(); r++)
            {
                unsigned char *row=img1.getRow(r);
                for(c=0;c<img1.width(); c++)
                    {
                        *row++=(unsigned char) r;
                        *row++=(unsigned char) r;
                        *row++=(unsigned char) r;
                    }
            }

        // do the same on img2, but using the
        // pixel function
        for(r=0; r<img2.height(); r++)
            for(c=0;c<img2.width(); c++)
                {
                    img2(c,r).r=r;
                    img2(c,r).g=r;
                    img2(c,r).b=r;
                }


        // now make sure the two images are the same
        int acc=0;
        for(r=0; r<img2.height(); r++)
            for(c=0;c<img2.width(); c++)
                {
                    acc+=(img2(c,r).r-img1(c,r).r);
                    acc+=(img2(c,r).g-img1(c,r).g);
                    acc+=(img2(c,r).b-img1(c,r).b);
                }

        checkEqual(acc,0,"pointer to row access");
    }

    // test const methods, this is mostly a compile time check.
    void testConstMethods()
    {
        report(0,"checking const methods...");
        ImageOf<PixelMono> img1;
        img1.resize(15,10);
        img1.zero();

        int r,c;
        int acc1=0;
        int acc2=0;
        for(r=0; r<img1.height(); r++)
            for(c=0;c<img1.width(); c++)
                {
                    img1(c,r)=(unsigned char) r;
                    acc1+=r;
                }

        const ImageOf<PixelMono> &constImg=img1;
        for(r=0; r<constImg.height(); r++)
            {
                const unsigned char *row=constImg.getRow(r);
                for(c=0;c<constImg.width(); c++)
                    {
                        const unsigned char &v1=constImg.pixel(c,r);
                        const unsigned char &v2=constImg(c,r);
                        acc1-=v1;
                        acc2+=v2;
                        row++;
                    }
            }

        checkEqual(acc1,0,"const methods");
    }


    void testBlank() {
        report(0,"checking blank images work (YARP bug 862810)...");
        ImageOf<PixelRgb> img;
        passImage(img);
    }

    void testRgba() {
        report(0,"checking rgba...");
        ImageOf<PixelRgba> img;
        ImageOf<PixelRgb> img2;
        img.resize(50,50);
        img(4,2) = PixelRgba(10,20,30,40);
        img2.copy(img);
        checkEqual(img(4,2).a,40,"a level");
        checkEqual(img2(4,2).r,10,"r level");
    }

    void testRgbInt() {
        report(0,"checking rgbi...");
        ImageOf<PixelRgbInt> img;
        ImageOf<PixelRgb> img2;
        img.resize(50,50);
        img(4,2) = PixelRgbInt(10,20,30);
        checkEqual(img(4,2).r,10,"r level original");
        img2.copy(img);
        checkEqual(img2(4,2).r,10,"r level copied");
    }

    void testOrigin() {
        report(0,"checking origin...");

        report(0, "checking origin external image...");
        unsigned char buf[EXT_HEIGHT][EXT_WIDTH];

        {
            for (int x=0; x<EXT_WIDTH; x++) {
                for (int y=0; y<EXT_HEIGHT; y++) {
                    buf[y][x] = 20;
                }
            }
        }

        ImageOf<PixelMono> img1;

        img1.setTopIsLowIndex(false);
        img1.setExternal(&buf[0][0],EXT_WIDTH,EXT_HEIGHT);

        checkEqual(img1.width(),EXT_WIDTH,"width check");
        checkEqual(img1.height(),EXT_HEIGHT,"height check");

        int mismatch = 0;
        for (int x=0; x<img1.width(); x++) {
            for (int y=0; y<img1.height(); y++) {
                img1.pixel(x,y) = 5;
                if (buf[img1.height()-y-1][x]!=5) {
                    mismatch++;
                }
            }
        }
        checkEqual(mismatch,0,"delta check");

        report(0, "checking origin with copy...");
        ImageOf<PixelInt> img2;
        ImageOf<PixelInt> img3;
        img2.setTopIsLowIndex(false);
        img2.setTopIsLowIndex(true);
        img2.resize(50,50);
        int ct = 1;
        for (int x=0; x<img2.width(); x++) {
            for (int y=0; y<img2.height(); y++) {
                img2(x,y) = ct;
                ct++;
            }
        }
        img3.copy(img2);
        mismatch = 0;
        ct = 1;
        for (int x=0; x<img2.width(); x++) {
            for (int y=0; y<img2.height(); y++) {
                if (img3(x,y)!=ct) {
                    mismatch++;
                }
                ct++;
            }
        }
        checkEqual(mismatch,0,"delta check");
    }


    void testExternalRepeat() {
        report(0,"checking that setExternal can be called multiple times...");

        unsigned char buf[EXT_HEIGHT*EXT_WIDTH*3];
        unsigned char buf2[EXT_HEIGHT*2*EXT_WIDTH*2*3];
        ImageOf<PixelRgb> img;

        img.setExternal(&buf[0],EXT_WIDTH,EXT_HEIGHT);

        checkEqual(img.width(),EXT_WIDTH,"width check");
        checkEqual(img.height(),EXT_HEIGHT,"height check");

        img.setExternal(&buf2[0],EXT_WIDTH*2,EXT_HEIGHT*2);

        checkEqual(img.width(),EXT_WIDTH*2,"width check");
        checkEqual(img.height(),EXT_HEIGHT*2,"height check");
    }


    virtual void runTests() {
        testCreate();
        bool netMode = Network::setLocalMode(true);
        testTransmit();
        Network::setLocalMode(netMode);
        testCopy();
        testCast();
        testExternal();
        testPadding();
        testZero();
        testStandard();
        testDraw();
        testScale();
        testRowPointer();
        testConstMethods();
        testBlank();
        testRgba();
        testRgbInt();
        testOrigin();
        testExternalRepeat();
    }
};

static ImageTest theImageTest;

UnitTest& getImageTest() {
    return theImageTest;
}


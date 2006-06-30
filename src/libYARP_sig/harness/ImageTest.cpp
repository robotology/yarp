// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 *
 * Tests for images
 *
 */

#include <yarp/NetType.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageDraw.h>
#include <yarp/os/Network.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::sig;
using namespace yarp::sig::draw;
using namespace yarp::os;

class ImageTest : public UnitTest {
public:
    virtual String getName() { return "ImageTest"; }

    void testCreate() {
        report(0,"testing image creation...");
        FlexImage image;
        image.setPixelCode(YARP_PIXEL_RGB);
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
        input.open("/in");
        output.open("/out");
        buf.attach(input);

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

        unsigned char buf2[13][5];
        ImageOf<PixelMono> img2;
        img2.setExternal(&buf2[0][0],13,5);
        checkEqual(img2.getQuantum(),1,"natural external quantum");
        checkEqual(img2.getRowSize(),13,"natural external row size");
        checkEqual(img2.width(),13,"natural external row width");

        unsigned char buf3[16][5];
        ImageOf<PixelMono> img3;
        img3.setQuantum(8);
        img3.setExternal(&buf3[0][0],13,5);
        checkEqual(img3.getQuantum(),8,"forced external quantum");
        checkEqual(img3.getRowSize(),16,"forced external row size");
        checkEqual(img3.width(),13,"normal external row width");
    }

    void testStandard() {
        report(0,"checking standard compliance of description...");
        ImageOf<PixelRgb> img;
        img.resize(8,4);
        BufferedConnectionWriter writer;
        img.write(writer);
        String s = writer.toString();
        Bottle bot;
        bot.fromBinary(s.c_str(),s.length());
        checkEqual(bot.size(),3,"plausible bottle out");
        checkEqual(bot.get(0).toString().c_str(),"img","good tag");
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
    }
};

static ImageTest theImageTest;

UnitTest& getImageTest() {
    return theImageTest;
}


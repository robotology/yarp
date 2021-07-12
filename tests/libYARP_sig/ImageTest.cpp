/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/NetType.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageDraw.h>
#include <yarp/sig/ImageUtils.h>
#include <yarp/os/Network.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/PeriodicThread.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::sig::draw;
using namespace yarp::os;

class readWriteTest : public yarp::os::PeriodicThread
{
    yarp::os::Port p;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> image;
public:
    readWriteTest() : PeriodicThread(0.01)
    {
    }

    yarp::sig::ImageOf<yarp::sig::PixelRgb> getImage()
    {
        return image;
    }

    virtual bool threadInit() override
    {
        p.open("/readWriteTest_writer");
        image.resize(100, 100);
        image.zero();
        return true;
    }

    virtual void threadRelease() override
    {
        p.close();
    }

    virtual void run() override
    {
        p.write(image);
        yarp::os::Time::delay(1);
    }
};

void passImage(ImageOf<PixelRgb> img)
{
    yInfo("passed a blank image ok");
}

TEST_CASE("sig::ImageTest", "[yarp::sig]")
{
    NetworkBase::setLocalMode(true);

    static const size_t EXT_WIDTH = 128;
    static const size_t EXT_HEIGHT = 64;

    SECTION("test image creation.")
    {
        FlexImage image;
        image.setPixelCode(VOCAB_PIXEL_RGB);
        image.resize(256,128);
        CHECK(image.width() ==  (size_t) 256); // check width
        CHECK(image.height() ==  (size_t) 128); // check height
        ImageOf<PixelInt> iint;
        iint.resize(256,128);
        long int total = 0;
        for (size_t x=0; x<iint.width(); x++) {
            for (size_t y=0; y<iint.height(); y++) {
                int v = (x+y)%65537;
                iint.pixel(x,y) = v;
                total += v;
            }
        }
        for (size_t x2=0; x2<iint.width(); x2++) {
            for (size_t y2=0; y2<iint.height(); y2++) {
                total -= iint.pixel(x2,y2);
            }
        }
        CHECK(total == 0); // pixel assignment check
    }

    SECTION("test Image::copy().")
    {
        constexpr size_t width = 128;
        constexpr size_t height = 64;
        ImageOf<PixelRgb> img1;
        img1.resize(width, height);

        for (size_t x = 0; x < img1.width(); x++) {
            for (size_t y = 0; y < img1.height(); y++) {
                PixelRgb& pixel = img1.pixel(x, y);
                pixel.r = x;
                pixel.g = y;
                pixel.b = 42;
            }
        }

        ImageOf<PixelRgb> result;
        result.copy(img1);

        CHECK(img1.width() == result.width()); // width check
        CHECK(img1.height() == result.height()); // height check
        if (img1.width() == result.width() && img1.height() == result.height()) {
            int mismatch = 0;
            for (size_t x = 0; x < img1.width(); x++) {
                for (size_t y = 0; y < img1.height(); y++) {
                    PixelRgb& pix0 = img1.pixel(x, y);
                    PixelRgb& pix1 = result.pixel(x, y);
                    if (pix0.r != pix1.r || pix0.g != pix1.g || pix0.b != pix1.b) {
                        mismatch++;
                    }
                }
            }
            CHECK(mismatch == 0); // pixel match check
        }
    }

    SECTION("test Image::move().")
    {
        constexpr size_t width = 128;
        constexpr size_t height = 64;
        ImageOf<PixelRgb> img1;
        img1.resize(width, height);

        for (size_t x = 0; x < img1.width(); x++) {
            for (size_t y = 0; y < img1.height(); y++) {
                PixelRgb& pixel = img1.pixel(x, y);
                pixel.r = x;
                pixel.g = y;
                pixel.b = 42;
            }
        }

        ImageOf<PixelRgb> result;
        result.move(std::move(img1));

        CHECK(result.width() == width); // width check
        CHECK(result.height() == height); // height check
        int mismatch = 0;
        for (size_t x = 0; x < result.width(); x++) {
            for (size_t y = 0; y < result.height(); y++) {
                PixelRgb& pix = result.pixel(x, y);
                if (pix.r != x || pix.g != y || pix.b != 42) {
                    mismatch++;
                }
            }
        }
        CHECK(mismatch == 0); // pixel match check
    }


    SECTION("test Image::swap().")
    {
        constexpr size_t width1 = 128;
        constexpr size_t height1 = 64;
        ImageOf<PixelRgb> img1;
        img1.resize(width1, height1);

        for (size_t x = 0; x < img1.width(); x++) {
            for (size_t y = 0; y < img1.height(); y++) {
                PixelRgb& pixel = img1.pixel(x, y);
                pixel.r = x;
                pixel.g = y;
                pixel.b = 42;
            }
        }

        constexpr size_t width2 = 64;
        constexpr size_t height2 = 32;
        ImageOf<PixelRgb> img2;
        img2.resize(width2, height2);

        for (size_t x = 0; x < img2.width(); x++) {
            for (size_t y = 0; y < img2.height(); y++) {
                PixelRgb& pixel = img2.pixel(x, y);
                pixel.r = y;
                pixel.g = x;
                pixel.b = 255 - 42;
            }
        }

        img2.swap(img1);

        // Check img1
        int mismatch = 0;
        CHECK(img1.width() == width2);
        CHECK(img1.height() == height2);
        for (size_t x = 0; x < img1.width(); x++) {
            for (size_t y = 0; y < img1.height(); y++) {
                PixelRgb& pix = img1.pixel(x, y);
                if (pix.r != y || pix.g != x || pix.b != 255 - 42) {
                    mismatch++;
                }
            }
        }
        CHECK(mismatch == 0); // pixel match check

        // Check img2
        mismatch = 0;
        CHECK(img2.width() == width1);
        CHECK(img2.height() == height1);
        for (size_t x = 0; x < img2.width(); x++) {
            for (size_t y = 0; y < img2.height(); y++) {
                PixelRgb& pix = img2.pixel(x, y);
                if (pix.r != x || pix.g != y || pix.b != 42) {
                    mismatch++;
                }
            }
        }
        CHECK(mismatch == 0); // pixel match check
    }

    SECTION("test image zeroing.")
    {
        ImageOf<PixelRgb> img1;
        img1.resize(128,64);
        img1.pixel(20,10).r = 42;
        CHECK(img1.pixel(20,10).r == 42); // pixel set
        img1.zero();
        CHECK(img1.pixel(20,10).r == 0); // pixel unset
    }

    SECTION("testing image casting...")
    {
        ImageOf<PixelRgb> img1;
        img1.resize(128,64);
        for (size_t x=0; x<img1.width(); x++) {
            for (size_t y=0; y<img1.height(); y++) {
                PixelRgb& pixel = img1.pixel(x,y);
                unsigned char v = x%30;
                pixel.r = v;
                pixel.g = v;
                pixel.b = v;
            }
        }

        ImageOf<PixelMono> result;
        result.copy(img1);

        CHECK(img1.width() == result.width()); // width check
        CHECK(img1.height() == result.height()); // height check

        if (img1.width()==result.width() &&
            img1.height()==result.height()) {
            int mismatch = 0;
            for (size_t x=0; x<img1.width(); x++) {
                for (size_t y=0; y<img1.height(); y++) {
                    PixelRgb& pix0 = img1.pixel(x,y);
                    PixelMono& pix1 = result.pixel(x,y);
                    if (pix0.r>pix1+1 || pix0.r<pix1-1) {
                        mismatch++;
                    }
                }
            }
            CHECK(mismatch==0); // pixel match check
        }
    }

    SECTION("test external image.")
    {
        unsigned char buf[EXT_HEIGHT][EXT_WIDTH];

        {
            for (size_t x=0; x<EXT_WIDTH; x++) {
                for (size_t y=0; y<EXT_HEIGHT; y++) {
                    buf[y][x] = 20;
                }
            }
        }

        ImageOf<PixelMono> img1;

        img1.setExternal(&buf[0][0],EXT_WIDTH,EXT_HEIGHT);

        CHECK(img1.width() == EXT_WIDTH); // width check
        CHECK(img1.height() == EXT_HEIGHT); // height check

        int mismatch = 0;
        for (size_t x=0; x<img1.width(); x++) {
            for (size_t y=0; y<img1.height(); y++) {
                img1.pixel(x,y) = 5;
                if (buf[y][x]!=5) {
                    mismatch++;
                }
            }
        }
        CHECK(mismatch == 0); // delta check

        INFO( "testing various padding + alignments...");
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


    SECTION("test image transmission.")
    {
        ImageOf<PixelRgb> img1;
        img1.resize(128,64);
        for (size_t x=0; x<img1.width(); x++) {
            for (size_t y=0; y<img1.height(); y++) {
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

        output.addOutput(Contact("/in", "tcp"));
        Time::delay(0.2);

        INFO("writing...");
        output.write(img1);
        output.write(img1);
        output.write(img1);
        INFO("reading...");
        ImageOf<PixelRgb> *result = buf.read();

        CHECK(result!=nullptr); // got something check
        if (result!=nullptr) {
            CHECK(img1.width() == result->width()); // width check
            CHECK(img1.height() == result->height()); // height check
            if (img1.width()==result->width() &&
                img1.height()==result->height()) {
                int mismatch = 0;
                for (size_t x=0; x<img1.width(); x++) {
                    for (size_t y=0; y<img1.height(); y++) {
                        PixelRgb& pix0 = img1.pixel(x,y);
                        PixelRgb& pix1 = result->pixel(x,y);
                        if (pix0.r!=pix1.r ||
                            pix0.g!=pix1.g ||
                            pix0.b!=pix1.b) {
                            mismatch++;
                        }
                    }
                }
                CHECK(mismatch==0); // pixel match check
            }
        }

        output.close();
        input.close();
    }


    SECTION("check image padding.")
    {
        ImageOf<PixelMono> img1;
        img1.resize(13,5);
        CHECK(img1.getQuantum() == (size_t) 8); // ipl compatible quantum
        CHECK(img1.getRowSize() == (size_t) 16); // ipl compatible row size
        CHECK(img1.width() == (size_t) 13); // good real row width
        CHECK(img1.getPadding() ==  img1.getRowSize()-img1.width()); // getPadding()

        unsigned char buf2[13][5];
        ImageOf<PixelMono> img2;
        img2.setExternal(&buf2[0][0],13,5);
        CHECK(img2.getQuantum() == (size_t) 1); // natural external quantum
        CHECK(img2.getRowSize() == (size_t) 13); // natural external row size
        CHECK(img2.width() == (size_t) 13); // natural external row width
        CHECK(img2.getPadding() ==  (size_t)  0); // natural external padding

        unsigned char buf3[16][5];
        ImageOf<PixelMono> img3;
        img3.setQuantum(8);
        img3.setExternal(&buf3[0][0],13,5);
        CHECK(img3.getQuantum() == (size_t) 8); // forced external quantum
        CHECK(img3.getRowSize() == (size_t) 16); // forced external row size
        CHECK(img3.width() == (size_t) 13); // normal external row width
        CHECK(img3.getPadding() == (size_t)  3); // forced external padding

        FlexImage img4;
        img4.setPixelCode(VOCAB_PIXEL_MONO);
        img4.setQuantum(1);
        img4.resize(10,10);
        CHECK(img4.getQuantum() == (size_t) 1); // unit quantum
        CHECK(img4.getRowSize() == (size_t) 10); // exact row size
    }

    SECTION("check standard compliance of description.")
    {
        ImageOf<PixelRgb> img;
        img.resize(8,4);
        img.zero();
        BufferedConnectionWriter writer;
        img.write(writer);
        std::string s = writer.toString();
        Bottle bot;
        bot.fromBinary(s.c_str(),s.length());
        CHECK(bot.size() == (size_t) 4); // plausible bottle out
        CHECK(bot.get(0).toString() == "mat"); // good tag
        INFO("an example image: " << bot.toString().c_str());
    }

    SECTION("check draw tools.")
    {
        ImageOf<PixelRgb> img;
        img.resize(64,64);
        img.zero();
        addCircle(img,PixelRgb{255,0,0},32,32,200);
        // full image should be colored blue
        bool ok = true;
        IMGFOR(img,x,y) {
            if (img.pixel(x,y).r!=255) {
                ok = false;
            }
        }
        CHECK(ok); // image is blue
    }

    SECTION("check scaling.")
    {
        ImageOf<PixelRgb> img;
        ImageOf<PixelMono> img2;
        ImageOf<PixelRgb> img3;
        img.resize(64,64);
        img.zero();
        for (size_t i=0; i<img.width()/2; i++) {
            for (size_t j=0; j<img.height()/2; j++) {
                img(i,j).r = 255;
                img(i,j).g = 255;
                img(i,j).b = 255;
            }
        }
        img2.copy(img,32,32);
        CHECK(img2.width() == (size_t) 32); // dimension check
        CHECK(img2(0,0) == 255);               // logic check
        CHECK(img2(img2.width()-2,0) == 0);    // logic check
        CHECK(img2(0,img2.height()-2) == 0);   // logic check
        img3.copy(img,16,16);
        CHECK(img3.width() == (size_t) 16); // dimension check
        CHECK(img3(0,0).r == 255); // logic check
        CHECK(img3(img3.width()-2,0).r == 0);  // logic check
        CHECK(img3(0,img3.height()-2).r == 0); // "logic check
        img.copy(img3,4,4);
        CHECK(img.width() == (size_t) 4); // dimension check
    }

    // test row pointer access (getRow())
    // this function only tests if getRow(r)[c] is consistent with the operator ()
    SECTION("check row pointer.")
    {
        ImageOf<PixelRgb> img1;
        ImageOf<PixelRgb> img2;

        img1.resize(59, 50);
        img2.resize(59, 50);

        img1.zero();
        img2.zero();

        size_t r,c;
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

        CHECK(acc == 0); // pointer to row access
    }

    // test const methods, this is mostly a compile time check.
    SECTION("check const methods.")
    {
        ImageOf<PixelMono> img1;
        img1.resize(15,10);
        img1.zero();

        size_t r,c;
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

        CHECK(acc1 == 0); // const methods
    }

    SECTION("checking blank images work (YARP bug 862810).")
    {
        ImageOf<PixelRgb> img;
        passImage(img);
    }

    SECTION("check rgba.")
    {
        ImageOf<PixelRgba> img;
        ImageOf<PixelRgb> img2;
        img.resize(50,50);
        img(4,2) = PixelRgba{10,20,30,40};
        img2.copy(img);
        CHECK(img(4,2).a == 40); // "a level"
        CHECK(img2(4,2).r == 10); // "r level"
    }

    SECTION("check rgbi.")
    {
        ImageOf<PixelRgbInt> img;
        ImageOf<PixelRgb> img2;
        img.resize(50,50);
        img(4,2) = PixelRgbInt{10,20,30};
        CHECK(img(4,2).r == 10); // r level original
        img2.copy(img);
        CHECK(img2(4,2).r == 10); // r level copied
    }

    SECTION("check origin.")
    {

        INFO("check origin external image.");
        unsigned char buf[EXT_HEIGHT][EXT_WIDTH];

        {
            for (size_t x=0; x<EXT_WIDTH; x++) {
                for (size_t y=0; y<EXT_HEIGHT; y++) {
                    buf[y][x] = 20;
                }
            }
        }

        ImageOf<PixelMono> img1;

        img1.setTopIsLowIndex(false);
        img1.setExternal(&buf[0][0],EXT_WIDTH,EXT_HEIGHT);

        CHECK(img1.width() == EXT_WIDTH); // width check
        CHECK(img1.height() == EXT_HEIGHT); // height check

        int mismatch = 0;
        for (size_t x=0; x<img1.width(); x++) {
            for (size_t y=0; y<img1.height(); y++) {
                img1.pixel(x,y) = 5;
                if (buf[img1.height()-y-1][x]!=5) {
                    mismatch++;
                }
            }
        }
        CHECK(mismatch == 0); // delta check

        INFO("check origin with copy...");
        ImageOf<PixelInt> img2;
        ImageOf<PixelInt> img3;
        img2.setTopIsLowIndex(false);
        img2.setTopIsLowIndex(true);
        img2.resize(50,50);
        int ct = 1;
        for (size_t x=0; x<img2.width(); x++) {
            for (size_t y=0; y<img2.height(); y++) {
                img2(x,y) = ct;
                ct++;
            }
        }
        img3.copy(img2);
        mismatch = 0;
        ct = 1;
        for (size_t x=0; x<img2.width(); x++) {
            for (size_t y=0; y<img2.height(); y++) {
                if (img3(x,y)!=ct) {
                    mismatch++;
                }
                ct++;
            }
        }
        CHECK(mismatch == 0); // delta check
    }

    SECTION("check that setExternal can be called multiple times.")
    {

        unsigned char buf[EXT_HEIGHT*EXT_WIDTH*3];
        unsigned char buf2[EXT_HEIGHT*2*EXT_WIDTH*2*3];
        ImageOf<PixelRgb> img;

        img.setExternal(&buf[0],EXT_WIDTH,EXT_HEIGHT);

        CHECK(img.width() == EXT_WIDTH); // width check
        CHECK(img.height() == EXT_HEIGHT); // height check

        img.setExternal(&buf2[0],EXT_WIDTH*2,EXT_HEIGHT*2);

        CHECK(img.width() == EXT_WIDTH*2); // width check
        CHECK(img.height() == EXT_HEIGHT*2); // height check
    }

    SECTION("readWrite test")
    {
        yarp::os::Network net;
        net.setLocalMode(true);
        yarp::os::Port p;
        readWriteTest writer;
        writer.start();
        p.open("/readWriteTest_reader");
        yarp::os::Network::connect("/readWriteTest_writer", "/readWriteTest_reader");
        yarp::sig::FlexImage im;
        p.read(im);
        yarp::sig::ImageOf<yarp::sig::PixelRgb> gtImage = writer.getImage();
        CHECK(im.getPixelSize() ==  gtImage.getPixelSize()); // checking flex image pixelsize after read
        CHECK(im.getPadding() ==  gtImage.getPadding()); // checking flex image padding after read
        CHECK(im.getPixelCode() ==  gtImage.getPixelCode()); // checking flex image pixel code after read
        CHECK(im.getQuantum() ==  gtImage.getQuantum()); // checking flex image quantum after read
        yarp::os::Network::disconnect("/readWriteTest_writer", "/readWriteTest_reader");
        writer.stop();
        p.close();

    }

    SECTION("Test split concatenation")
    {
        INFO("Horizontal concatenation");
        ImageOf<PixelRgb> imL, imR;
        imL.resize(8,4);
        imR.resize(8,4);

        // Prepare the images
        for (size_t i=0; i<imL.width(); ++i) {
            for (size_t j=0; j<imL.height(); ++j) {
                imL.pixel(i,j).r=255;
                imL.pixel(i,j).g=0;
                imL.pixel(i,j).b=0;
            }
        }

        for (size_t i=0; i<imR.width(); ++i) {
            for (size_t j=0; j<imR.height(); ++j) {
                imR.pixel(i,j).r=0;
                imR.pixel(i,j).g=0;
                imR.pixel(i,j).b=255;
            }
        }

        ImageOf<PixelRgb> bigImg;
        CHECK(utils::horzConcat(imL, imR, bigImg)); // Check horizontal concatenation
        CHECK(bigImg.width() ==  imL.width()*2); // Checking width of the big image
        CHECK(bigImg.height() ==  imL.height()); // Checking height of the big image

        bool ok = true;
        for (size_t i=0; i<bigImg.width(); ++i) {
            for (size_t j=0; j<bigImg.height(); ++j) {
                if (i<imL.width()) {
                    ok &= bigImg.pixel(i,j).r == 255;
                    ok &= bigImg.pixel(i,j).g == 0;
                    ok &= bigImg.pixel(i,j).b == 0;
                }
                else {
                    ok &= bigImg.pixel(i,j).r == 0;
                    ok &= bigImg.pixel(i,j).g == 0;
                    ok &= bigImg.pixel(i,j).b == 255;
                }
            }
        }
        CHECK(ok); // Checking data consistency

        INFO( "Vertical split");

        ImageOf<PixelRgb> splitL, splitR;

        CHECK(utils::vertSplit(bigImg, splitL, splitR)); // Check vertical split
        CHECK(splitL.width() ==  imL.width()); // Check width left split
        CHECK(splitR.width() ==  imR.width()); // Check width right split
        CHECK(splitL.height() ==  imL.height()); // Check height left split
        CHECK(splitR.height() ==  imR.height()); // Check height right split

        ok = true;

        for (size_t i=0; i<imL.width(); ++i) {
            for (size_t j=0; j<imL.height(); ++j) {
                ok &= imL.pixel(i,j).r == splitL.pixel(i,j).r;
                ok &= imL.pixel(i,j).g == splitL.pixel(i,j).g;
                ok &= imL.pixel(i,j).b == splitL.pixel(i,j).b;
            }
        }

        CHECK(ok); // Checking data consistency left split
        ok = true;

        for (size_t i=0; i<imR.width(); ++i) {
            for (size_t j=0; j<imR.height(); ++j) {
                ok &= imR.pixel(i,j).r == splitR.pixel(i,j).r;
                ok &= imR.pixel(i,j).g == splitR.pixel(i,j).g;
                ok &= imR.pixel(i,j).b == splitR.pixel(i,j).b;
            }
        }

        CHECK(ok); // Checking data consistency right split


        INFO( "Vertical concatenation");

        ImageOf<PixelRgb> imUp(imL);
        ImageOf<PixelRgb> imDown(imR);
        bigImg.zero();

        CHECK(utils::vertConcat(imUp, imDown, bigImg)); // Check vertical concatenation
        CHECK(bigImg.width() ==  imUp.width() ); // Checking width of the big image
        CHECK(bigImg.height() ==  imDown.height()*2); // Checking height of the big image

        ok = true;
        for (size_t i=0; i<bigImg.width(); ++i) {
            for (size_t j=0; j<bigImg.height(); ++j) {
                if (j<imUp.height()) {
                    ok &= bigImg.pixel(i,j).r == 255;
                    ok &= bigImg.pixel(i,j).g == 0;
                    ok &= bigImg.pixel(i,j).b == 0;
                }
                else {
                    ok &= bigImg.pixel(i,j).r == 0;
                    ok &= bigImg.pixel(i,j).g == 0;
                    ok &= bigImg.pixel(i,j).b == 255;
                }
            }
        }

        CHECK(ok); // Checking data consistency

        INFO( "Horizontal split");

        ImageOf<PixelRgb> splitUp, splitDown;

        CHECK(utils::horzSplit(bigImg, splitUp, splitDown)); // Check horizontal split
        CHECK(splitUp.width() ==  imUp.width()); // Check width top split
        CHECK(splitDown.width() ==  imDown.width()); // Check width bottom split
        CHECK(splitUp.height() ==  imUp.height()); // Check height top split
        CHECK(splitDown.height() ==  imDown.height()); // Check height bottom split

        ok = true;

        for (size_t i=0; i<imUp.width(); ++i) {
            for (size_t j=0; j<imUp.height(); ++j) {
                ok &= imUp.pixel(i,j).r == splitUp.pixel(i,j).r;
                ok &= imUp.pixel(i,j).g == splitUp.pixel(i,j).g;
                ok &= imUp.pixel(i,j).b == splitUp.pixel(i,j).b;
            }
        }

        CHECK(ok); // Checking data consistency top split
        ok = true;

        for (size_t i=0; i<imDown.width(); ++i) {
            for (size_t j=0; j<imDown.height(); ++j) {
                ok &= imDown.pixel(i,j).r == splitDown.pixel(i,j).r;
                ok &= imDown.pixel(i,j).g == splitDown.pixel(i,j).g;
                ok &= imDown.pixel(i,j).b == splitDown.pixel(i,j).b;
            }
        }

        CHECK(ok); // Checking data consistency bottom split
    }

    SECTION("test image crop.")
    {
        ImageOf<PixelRgb> inImg, outImg;
        inImg.resize(10, 10);

        size_t tlx = 4;
        size_t tly = 5;
        size_t brx = 8;
        size_t bry = 7;

        PixelRgb pixelValue {255, 0, 0};

        for (size_t u = tlx; u <= brx; u++) {
            for (size_t v = tly; v <= bry; v++) {
                inImg.pixel(u, v) = pixelValue;
            }
        }

        CHECK(utils::cropRect(inImg, {tlx, tly}, {brx, bry}, outImg));

        CHECK(outImg.width() == brx - tlx + 1);
        CHECK(outImg.height() == bry - tly + 1);

        bool ok = true;

        for (size_t u = 0; u < outImg.width(); u++) {
            for (size_t v = 0; v < outImg.height(); v++) {
                ok &= outImg.pixel(u, v).r == pixelValue.r;
                ok &= outImg.pixel(u, v).g == pixelValue.g;
                ok &= outImg.pixel(u, v).b == pixelValue.b;
            }
        }

        CHECK(ok);
    }

    NetworkBase::setLocalMode(false);
}

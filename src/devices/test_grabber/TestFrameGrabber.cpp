/*
 * Copyright (C) 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <TestFrameGrabber.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/sig/ImageDraw.h>
#include <yarp/os/Random.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::sig::draw;

#define VOCAB_BALL VOCAB4('b','a','l','l')
#define VOCAB_GRID VOCAB4('g','r','i','d')
#define VOCAB_RAND VOCAB4('r','a','n','d')
#define VOCAB_NONE VOCAB4('n','o','n','e')
#define VOCAB_GRID_MULTISIZE VOCAB4('s','i','z','e')
TestFrameGrabber::TestFrameGrabber() {
    ct = 0;
    freq = 30;
    period = 1/freq;
    // just for nostalgia
    w = 128;
    h = 128;
    first = 0;
    prev = 0;
    rnd = 0;
    use_bayer = false;
    use_mono = false;
    mirror=false;
    horizontalFov=0.0;
    verticalFov=0.0;
}


bool TestFrameGrabber::close() {
    return true;
}

bool TestFrameGrabber::open(yarp::os::Searchable& config) {
    yarp::os::Value *val;
    Value* retM;
    retM=Value::makeList("1.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0");
    w = config.check("width",yarp::os::Value(320),
                     "desired width of test image").asInt();
    h = config.check("height",yarp::os::Value(240),
                     "desired height of test image").asInt();
    horizontalFov=config.check("horizontalFov",Value(1.0),
                               "desired horizontal fov of test image").asDouble();
    verticalFov=config.check("verticalFov",Value(2.0),
                               "desired vertical fov of test image").asDouble();
    mirror=config.check("mirror",Value(false),
                        "mirroring disabled by default").asBool();
    intrinsic.put("focalLengthX",config.check("focalLengthX",Value(4.0),"Horizontal component of the focal lenght of the test_grabber").asDouble());
    intrinsic.put("focalLengthY",config.check("focalLengthY",Value(5.0),"Vertical component of the focal lenght of the test_grabber").asDouble());
    intrinsic.put("principalPointX",config.check("principalPointX",Value(6.0),"X coordinate of the principal point of the test_grabber").asDouble());
    intrinsic.put("principalPointY",config.check("principalPointY",Value(7.0),"Y coordinate of the principal point of the test_grabber").asDouble());
    intrinsic.put("retificationMatrix",config.check("retificationMatrix",*retM,"Matrix that describes the lens' distortion(fake)"));
    intrinsic.put("distortionModel",config.check("distortionModel",Value("FishEye"),"Reference to group of parameters describing the distortion model of the camera").asString());
    intrinsic.put("k1",config.check("k1",Value(8.0),"Radial distortion coefficient of the lens(fake)").asDouble());
    intrinsic.put("k2",config.check("k2",Value(9.0),"Radial distortion coefficient of the lens(fake)").asDouble());
    intrinsic.put("k3",config.check("k3",Value(10.0),"Radial distortion coefficient of the lens(fake)").asDouble());
    intrinsic.put("t1",config.check("t1",Value(11.0),"Tangential distortion of the lens(fake)").asDouble());
    intrinsic.put("t2",config.check("t2",Value(12.0),"Tangential distortion of the lens(fake)").asDouble());
    //Only for debug
    CameraConfig conf1, conf2, conf3;
    conf1.height=128; conf1.width=128; conf1.framerate=60.0; conf1.pixelCoding=VOCAB_PIXEL_RGB;
    conf2.height=256; conf2.width=256; conf2.framerate=30.0; conf2.pixelCoding=VOCAB_PIXEL_BGR;
    conf3.height=512; conf3.width=512; conf3.framerate=15.0; conf3.pixelCoding=VOCAB_PIXEL_MONO;
    configurations.push_back(conf1);
    configurations.push_back(conf2);
    configurations.push_back(conf3);
    delete retM;

    if (config.check("freq",val,"rate of test images in Hz")) {
        freq = val->asDouble();
        period = 1/freq;
    } else if (config.check("period",val,
                            "period of test images in seconds")) {
        period = val->asDouble();
        if(period<=0) {
            period =0;
            freq = -1;
        }
    }
    mode = config.check("mode",
                        yarp::os::Value(VOCAB_LINE, true),
                        "bouncy [ball], scrolly [line], grid [grid], grid multisize [size], random [rand], none [none]").asVocab();

    if (config.check("src")) {
        if (!yarp::sig::file::read(background,
                                   config.check("src",
                                                yarp::os::Value("test.ppm"),
                                                "background image to use, if any").asString().c_str())) {
            return false;
        }
        if (background.width()>0) {
            w = background.width();
            h = background.height();
        }
    }

    use_bayer = config.check("bayer","should emit bayer test image?");
    use_mono = config.check("mono","should emit a monochrome image?");
    use_mono = use_mono||use_bayer;

    if (freq!=-1) {
        yInfo("Test grabber period %g / freq %g , mode [%s]", period, freq,
               yarp::os::Vocab::decode(mode).c_str());
    } else {
        yInfo("Test grabber period %g / freq [inf], mode [%s]", period,
               yarp::os::Vocab::decode(mode).c_str());
    }

    bx = w/2;
    by = h/2;
    return true;
}

void TestFrameGrabber::timing() {
    double now = yarp::os::Time::now();

    if (now-prev>1000) {
        first = now;
        prev = now;
    }
    double dt = period-(now-prev);

    if (dt>0) {
        yarp::os::Time::delay(dt);
    }

    // this is the controlled instant when we consider the
    // image as going out
    prev += period;
}

int TestFrameGrabber::height() const {
    return h;
}

int TestFrameGrabber::width() const {
    return w;
}

int TestFrameGrabber::getRgbHeight(){
    return h;
}

int TestFrameGrabber::getRgbWidth(){
    return w;
}

bool TestFrameGrabber::getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations){
    configurations=this->configurations;
    return true;
}

bool TestFrameGrabber::getRgbResolution(int &width, int &height){
    width=w;
    height=h;
    return true;
}

bool TestFrameGrabber::setRgbResolution(int width, int height){
    w=width;
    h=height;
    return true;
}

bool TestFrameGrabber::getRgbFOV(double &horizontalFov, double &verticalFov){
    horizontalFov=this->horizontalFov;
    verticalFov=this->verticalFov;
    return true;
}

bool TestFrameGrabber::setRgbFOV(double horizontalFov, double verticalFov){
    this->horizontalFov=horizontalFov;
    this->verticalFov=verticalFov;
    return true;
}

bool TestFrameGrabber::getRgbIntrinsicParam(yarp::os::Property &intrinsic){
    intrinsic=this->intrinsic;
    return true;
}

bool TestFrameGrabber::getRgbMirroring(bool &mirror){
    mirror=this->mirror;
    return true;}

bool TestFrameGrabber::setRgbMirroring(bool mirror){
    this->mirror=mirror;
    return true;
}

bool TestFrameGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
    timing();
    createTestImage(image);
    return true;
}


bool TestFrameGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) {
    timing();
    createTestImage(rgb_image);
    if (use_bayer) {
        makeSimpleBayer(rgb_image,image);
    } else {
        image.copy(rgb_image);
    }
    return true;
}

bool TestFrameGrabber::setBrightness(double v) {
    return false;
}
bool TestFrameGrabber::setExposure(double v) {
    return false;
}
bool TestFrameGrabber::setSharpness(double v) {
    return false;
}
bool TestFrameGrabber::setWhiteBalance(double blue, double red){
    return false;
}
bool TestFrameGrabber::setHue(double v) {
    return false;
}
bool TestFrameGrabber::setSaturation(double v) {
    return false;
}
bool TestFrameGrabber::setGamma(double v) {
    return false;
}
bool TestFrameGrabber::setShutter(double v) {
    return false;
}
bool TestFrameGrabber::setGain(double v) {
    return false;
}
bool TestFrameGrabber::setIris(double v) {
    return false;
}

double TestFrameGrabber::getBrightness(){
    return 0.0;
}
double TestFrameGrabber::getExposure(){
    return 0.0;
}
double TestFrameGrabber::getSharpness(){
    return 0.0;
}
bool TestFrameGrabber::getWhiteBalance(double &blue, double &red)
{
    red=0.0;
    blue=0.0;
    return true;
}
double TestFrameGrabber::getHue(){
    return 0.0;
}
double TestFrameGrabber::getSaturation(){
    return 0.0;
}
double TestFrameGrabber::getGamma(){
    return 0.0;
}
double TestFrameGrabber::getShutter(){
    return 0.0;
}
double TestFrameGrabber::getGain(){
    return 0.0;
}
double TestFrameGrabber::getIris(){
    return 0.0;
}

yarp::os::Stamp TestFrameGrabber::getLastInputStamp() {
    return stamp;
}

bool TestFrameGrabber::hasAudio() { return false; }

bool TestFrameGrabber::hasVideo() { return !use_mono; }

bool TestFrameGrabber::hasRawVideo() {
    return use_mono;
}

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
        image.resize(w,h);
        image.zero();
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
    case VOCAB_GRID_MULTISIZE:
    {
        static int count = 0;
        count++;
        if (count== 100)
        {
            yDebug() << "size 100, 100";
            image.resize(100,100);
        }
        else if (count == 200)
        {
            yDebug() << "size 200, 100";
            image.resize(200, 100);
        }
        else if (count == 300)
        {
            yDebug() << "size 300, 50";
            image.resize(300, 50);
            count = 0;
        }
        
        int ww = w = image.width();
        int hh = h = image.height();
        if (ww>1 && hh>1) {
            for (int x = 0; x<ww; x++) {
                for (int y = 0; y<hh; y++) {
                    double xx = ((double)x) / (ww - 1);
                    double yy = ((double)y) / (hh - 1);
                    int r = int(0.5 + 255 * xx);
                    int g = int(0.5 + 255 * yy);
                    bool act = (y == ct);
                    image.pixel(x, y) = PixelRgb(r, g, act * 255);
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
            sprintf(ttxt, "%021.10f", t);
            image.pixel(0, 0).r = ttxt[0] - '0';
            image.pixel(0, 0).g = ttxt[1] - '0';
            image.pixel(0, 0).b = ttxt[2] - '0';

            image.pixel(1, 0).r = ttxt[3] - '0';
            image.pixel(1, 0).g = ttxt[4] - '0';
            image.pixel(1, 0).b = ttxt[5] - '0';

            image.pixel(2, 0).r = ttxt[6] - '0';
            image.pixel(2, 0).g = ttxt[7] - '0';
            image.pixel(2, 0).b = ttxt[8] - '0';

            image.pixel(3, 0).r = ttxt[9] - '0';
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

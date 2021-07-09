/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeFrameGrabber.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/ImageDraw.h>

#include <cstdio>
#include <thread>
#include <random>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::sig::draw;

namespace {
YARP_LOG_COMPONENT(FAKEFRAMEGRABBER, "yarp.device.fakeFrameGrabber")
constexpr yarp::conf::vocab32_t VOCAB_LINE           = yarp::os::createVocab32('l','i','n','e');
constexpr yarp::conf::vocab32_t VOCAB_BALL           = yarp::os::createVocab32('b','a','l','l');
constexpr yarp::conf::vocab32_t VOCAB_GRID           = yarp::os::createVocab32('g','r','i','d');
constexpr yarp::conf::vocab32_t VOCAB_RAND           = yarp::os::createVocab32('r','a','n','d');
constexpr yarp::conf::vocab32_t VOCAB_NONE           = yarp::os::createVocab32('n','o','n','e');
constexpr yarp::conf::vocab32_t VOCAB_GRID_MULTISIZE = yarp::os::createVocab32('s','i','z','e');
constexpr yarp::conf::vocab32_t VOCAB_TIMETEXT       = yarp::os::createVocab32('t','i','m','e');

//the following data are used by [time] test
constexpr char num[12][16]
{
    // '0'
    "***"
    "* *"
    "* *"
    "* *"
    "***",

    // '1'
    " * "
    " * "
    " * "
    " * "
    " * ",

    // '2'
    "***"
    "  *"
    "***"
    "*  "
    "***",

    // '3'
    "***"
    "  *"
    "***"
    "  *"
    "***",

    // '4'
    "* *"
    "* *"
    "***"
    "  *"
    "  *",

    // '5'
    "***"
    "*  "
    "***"
    "  *"
    "***",

    // '6'
    "***"
    "*  "
    "***"
    "* *"
    "***",

    // '7'
    "***"
    "  *"
    "  *"
    "  *"
    "  *",

    // '8'
    "***"
    "* *"
    "***"
    "* *"
    "***",

    // '9'
    "***"
    "* *"
    "***"
    "  *"
    "***",

    // ' '
    "   "
    "   "
    "   "
    "   "
    "   ",

    // '.'
    "   "
    "   "
    "   "
    " **"
    " **",
};
constexpr size_t num_width = 3;
constexpr size_t num_height = 5;

}

bool FakeFrameGrabber::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) return false;
    reply.clear();

    if (command.get(0).asString()=="help")
    {
        reply.addVocab32("many");
        reply.addString("set_mode <mode>");
        reply.addString("set_image <file_name>/off");
        reply.addString("available modes: ball, line, grid, size, rand, none, time");
        reply.addString("set_topIsLow on/off");
        reply.addString("set_noise on/off");
        reply.addString("set_snr <snr>");
        reply.addString("");
    }
    else if (command.get(0).asString() == "set_mode")
    {
        mode= command.get(1).asVocab32();
        reply.addString("ack");
    }
    else if (command.get(0).asString() == "set_image")
    {
        if (command.get(1).asString() == "off")
        {
            have_bg=false;
            reply.addString("ack");
        }
        else
        {
            if (yarp::sig::file::read(background, command.get(1).asString()))
            {
                w = background.width();
                h = background.height();
                have_bg = true;
                reply.addString("ack");
            }
            else
            {
                have_bg = false;
                reply.addString("err");
            }
        }
    }
    else if (command.get(0).asString() == "set_topIsLow")
    {
        if (command.get(1).asString() == "off") {
            topIsLow = false;
            reply.addString("ack");
        } else if (command.get(1).asString() == "on") {
            topIsLow = true;
            reply.addString("ack");
        } else {
            reply.addString("err");
        }
    }
    else if (command.get(0).asString() == "set_noise")
    {
        if (command.get(1).asString() == "off") {
            add_noise = false;
            reply.addString("ack");
        } else if (command.get(1).asString() == "on") {
            add_noise = true;
            reply.addString("ack");
        } else {
            reply.addString("err");
        }
    }
    else if (command.get(0).asString() == "set_snr")
    {
        snr = yarp::conf::clamp(command.get(1).asFloat64(), 0.0, 1.0);
        reply.addString("ack");
    }
    else
    {
        reply.addString("Unknown command. Type 'help'.");
    }

    yarp::os::ConnectionWriter* returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }

    return true;
}

bool FakeFrameGrabber::close() {
    stop();
    m_rpcPort.close();
    return true;
}

bool FakeFrameGrabber::open(yarp::os::Searchable& config) {
    m_rpcPortName = config.check("fakeFrameGrabber_rpc_port", yarp::os::Value("/fakeFrameGrabber/rpc"), "rpc port for the fakeFrameGrabber").asString();
    w = config.check("width",yarp::os::Value(320),
                     "desired width of test image").asInt32();
    h = config.check("height",yarp::os::Value(240),
                     "desired height of test image").asInt32();
    horizontalFov=config.check("horizontalFov",Value(1.0),
                               "desired horizontal fov of test image").asFloat64();
    verticalFov=config.check("verticalFov",Value(2.0),
                               "desired vertical fov of test image").asFloat64();
    mirror=config.check("mirror",Value(false),
                        "mirroring disabled by default").asBool();
    syncro=config.check("syncro",Value(false),
                        "syncronize producer and consumer, so that all images are used once and only once").asBool();
    topIsLow=config.check("topIsLow",Value(true),
                          "explicitly set the topIsLow field in the images").asBool();
    intrinsic.put("physFocalLength",config.check("physFocalLength",Value(3.0),"Physical focal length of the fakeFrameGrabber").asFloat64());
    intrinsic.put("focalLengthX",config.check("focalLengthX",Value(4.0),"Horizontal component of the focal length of the fakeFrameGrabber").asFloat64());
    intrinsic.put("focalLengthY",config.check("focalLengthY",Value(5.0),"Vertical component of the focal length of the fakeFrameGrabber").asFloat64());
    intrinsic.put("principalPointX",config.check("principalPointX",Value(6.0),"X coordinate of the principal point of the fakeFrameGrabber").asFloat64());
    intrinsic.put("principalPointY",config.check("principalPointY",Value(7.0),"Y coordinate of the principal point of the fakeFrameGrabber").asFloat64());

    Value* retM;
    retM=Value::makeList("1.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0");
    intrinsic.put("rectificationMatrix",config.check("rectificationMatrix",*retM,"Matrix that describes the lens' distortion(fake)"));
    delete retM;

    intrinsic.put("distortionModel",config.check("distortionModel",Value("FishEye"),"Reference to group of parameters describing the distortion model of the camera").asString());
    intrinsic.put("k1",config.check("k1",Value(8.0),"Radial distortion coefficient of the lens(fake)").asFloat64());
    intrinsic.put("k2",config.check("k2",Value(9.0),"Radial distortion coefficient of the lens(fake)").asFloat64());
    intrinsic.put("k3",config.check("k3",Value(10.0),"Radial distortion coefficient of the lens(fake)").asFloat64());
    intrinsic.put("t1",config.check("t1",Value(11.0),"Tangential distortion of the lens(fake)").asFloat64());
    intrinsic.put("t2",config.check("t2",Value(12.0),"Tangential distortion of the lens(fake)").asFloat64());
    //Only for debug
    CameraConfig conf1;
    conf1.height=128;
    conf1.width=128;
    conf1.framerate=60.0;
    conf1.pixelCoding=VOCAB_PIXEL_RGB;
    configurations.push_back(conf1);

    CameraConfig conf2;
    conf2.height=256;
    conf2.width=256;
    conf2.framerate=30.0;
    conf2.pixelCoding=VOCAB_PIXEL_BGR;
    configurations.push_back(conf2);

    CameraConfig conf3;
    conf3.height=512;
    conf3.width=512;
    conf3.framerate=15.0;
    conf3.pixelCoding=VOCAB_PIXEL_MONO;
    configurations.push_back(conf3);

    yarp::os::Value *val;
    if (config.check("freq", val, "rate of test images in Hz")) {
        freq = val->asFloat64();
        period = 1/freq;
    } else if (config.check("period", val,
                            "period of test images in seconds")) {
        period = val->asFloat64() / 1000.0;
        if(period<=0) {
            period =0;
            freq = -1;
        }
    }

    mode = config.check("mode",
                        yarp::os::Value(VOCAB_LINE, true),
                        "bouncy [ball], scrolly [line], grid [grid], grid multisize [size], random [rand], none [none], time test[time]").asVocab32();

    if (config.check("src")) {
        if (!yarp::sig::file::read(background,
                                   config.check("src",
                                                yarp::os::Value("test.ppm"),
                                                "background image to use, if any").asString())) {
            return false;
        }
        if (background.width()>0) {
            if (config.check("width") || config.check("height")) {
                yCWarning(FAKEFRAMEGRABBER, "width and height option are ignored when passing a background image");
            }
            w = background.width();
            h = background.height();
            have_bg = true;
        }
    }

    add_timestamp = config.check("timestamp", "should write the timestamp in the first bytes of the image");

    add_noise = config.check("noise", "Should add noise to the image (uses snr)");

    snr = yarp::conf::clamp(config.check("snr",Value(default_snr), "Signal noise ratio ([0.0-1.0] default 0.5)").asFloat64(), 0.0, 1.0);

    use_bayer = config.check("bayer","should emit bayer test image?");
    use_mono = config.check("mono","should emit a monochrome image?");
    use_mono = use_mono||use_bayer;

    if (freq!=-1) {
        yCInfo(FAKEFRAMEGRABBER,
               "Test grabber period %g / freq %g , mode [%s]",
               period,
               freq,
               yarp::os::Vocab32::decode(mode).c_str());
    } else {
        yCInfo(FAKEFRAMEGRABBER,
               "Test grabber period %g / freq [inf], mode [%s]",
               period,
               yarp::os::Vocab32::decode(mode).c_str());
    }

    bx = w/2;
    by = h/2;

    for (auto& buff : buffs) {
        buff.resize(w, h);
        buff.zero();
    }

    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yCError(FAKEFRAMEGRABBER, "Failed to open port %s", m_rpcPortName.c_str());
        yCError(FAKEFRAMEGRABBER, "Do you have multiple FakeFrameGrabber devices running?");
        yCError(FAKEFRAMEGRABBER, "If yes, use the `fakeFrameGrabber_rpc_port` parameter to set a different name for each of them");
        return false;
    }
    m_rpcPort.setReader(*this);

    start();

    return true;
}

void FakeFrameGrabber::timing() {
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

int FakeFrameGrabber::height() const {
    return h;
}

int FakeFrameGrabber::width() const {
    return w;
}

int FakeFrameGrabber::getRgbHeight(){
    return h;
}

int FakeFrameGrabber::getRgbWidth(){
    return w;
}

bool FakeFrameGrabber::getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations){
    configurations=this->configurations;
    return true;
}

bool FakeFrameGrabber::getRgbResolution(int &width, int &height){
    width=w;
    height=h;
    return true;
}

bool FakeFrameGrabber::setRgbResolution(int width, int height){
    w=width;
    h=height;
    return true;
}

bool FakeFrameGrabber::getRgbFOV(double &horizontalFov, double &verticalFov){
    horizontalFov=this->horizontalFov;
    verticalFov=this->verticalFov;
    return true;
}

bool FakeFrameGrabber::setRgbFOV(double horizontalFov, double verticalFov){
    this->horizontalFov=horizontalFov;
    this->verticalFov=verticalFov;
    return true;
}

bool FakeFrameGrabber::getRgbIntrinsicParam(yarp::os::Property &intrinsic){
    intrinsic=this->intrinsic;
    return true;
}

bool FakeFrameGrabber::getRgbMirroring(bool &mirror){
    mirror=this->mirror;
    return true;}

bool FakeFrameGrabber::setRgbMirroring(bool mirror){
    this->mirror=mirror;
    return true;
}

void FakeFrameGrabber::run()
{
    while (!isStopping()) {
        for (size_t i = 0; i < 2 && !isStopping(); ++i) {
            if (!syncro) {
                std::unique_lock<std::mutex> lk(mutex[i]);
                createTestImage(buffs[i], buff_ts[i]);
                timing();
                lk.unlock();

                curr_buff_mutex.lock();
                curr_buff = i;
                curr_buff_mutex.unlock();
                std::this_thread::yield();
            } else {
                std::unique_lock<std::mutex> lk(mutex[i]);
                img_consumed_cv[i].wait(lk, [&]{ if (img_ready[i]) {img_ready_cv[i].notify_one();} return (isStopping() || img_consumed[i]);});
                if (isStopping()) {
                    break;
                }
                img_ready[i] = false;
                img_consumed[i] = false;
                createTestImage(buffs[i], buff_ts[i]);
                img_ready[i] = true;
                img_ready_cv[i].notify_all();
            }
        }
    }
}

void FakeFrameGrabber::onStop()
{
    // Unlock any blocked thread.
    if (syncro) {
        for (size_t i = 0; i < 2; ++i) {
            std::unique_lock<std::mutex> lk(mutex[i]);
            img_consumed[i] = true;
            img_consumed_cv[i].notify_all();
            img_ready[i] = true;
            img_ready_cv[i].notify_all();
        }
    }
}


bool FakeFrameGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    if (!isRunning()) {
        return false;
    }

    if (!syncro) {
        curr_buff_mutex.lock();
        size_t cb = curr_buff;
        std::unique_lock<std::mutex> lk(mutex[cb]);
        // Release the mutex after we get the lock on current image to avoid
        // that the image is swapped while we are waiting to read it
        curr_buff_mutex.unlock();
        image.copy(buffs[cb]);
        stamp.update(buff_ts[cb]);
    } else {
        curr_buff_mutex.lock();
        timing();
        size_t cb = curr_buff;
        std::unique_lock<std::mutex> lk(mutex[cb]);
        img_ready_cv[cb].wait(lk, [&]{return (!isRunning() || img_ready[cb]);});
        if (!isRunning()) {
            return false;
        }

        image.copy(buffs[cb]);
        stamp.update(buff_ts[cb]);
        img_consumed[cb] = true;
        img_consumed_cv[cb].notify_one();

        curr_buff = (cb + 1) % 2;
        curr_buff_mutex.unlock();
    }

    return true;
}

bool FakeFrameGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
{
    if (!isRunning()) {
        return false;
    }

    if (!syncro) {
        curr_buff_mutex.lock();
        size_t cb = curr_buff;
        std::unique_lock<std::mutex> lk(mutex[cb]);
        curr_buff_mutex.unlock();
        if (use_bayer) {
            makeSimpleBayer(buffs[cb], image);
        } else {
            image.copy(buffs[cb]);
        }
        stamp.update(buff_ts[cb]);
    } else {
        curr_buff_mutex.lock();
        timing();
        size_t cb = curr_buff;
        std::unique_lock<std::mutex> lk(mutex[cb]);
        img_ready_cv[cb].wait(lk, [&]{return (!isRunning() || img_ready[cb]);});
        if (!isRunning()) {
            return false;
        }
        if (use_bayer) {
            makeSimpleBayer(buffs[cb], image);
        } else {
            image.copy(buffs[cb]);
        }
        stamp.update(buff_ts[cb]);
        img_consumed[cb] = true;
        img_consumed_cv[cb].notify_one();

        curr_buff = (cb + 1) % 2;
        curr_buff_mutex.unlock();
    }

    return true;
}

bool FakeFrameGrabber::getImageCrop(cropType_id_t cropType,
                                    yarp::sig::VectorOf<std::pair<int, int>> vertices,
                                    yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    yCDebugThrottle(FAKEFRAMEGRABBER, 5.0) << "Hardware crop requested!";
    return yarp::dev::IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::getImageCrop(cropType, vertices, image);
}

bool FakeFrameGrabber::getImageCrop(cropType_id_t cropType,
                                    yarp::sig::VectorOf<std::pair<int, int>> vertices,
                                    yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
{
    yCDebugThrottle(FAKEFRAMEGRABBER, 5.0) << "Hardware crop requested!";
    return yarp::dev::IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>::getImageCrop(cropType, vertices, image);
}

yarp::os::Stamp FakeFrameGrabber::getLastInputStamp() {
    return stamp;
}

bool FakeFrameGrabber::hasAudio() { return false; }

bool FakeFrameGrabber::hasVideo() { return !use_mono; }

bool FakeFrameGrabber::hasRawVideo() {
    return use_mono;
}

bool FakeFrameGrabber::getCameraDescription(CameraDescriptor *camera) { return false; }
bool FakeFrameGrabber::hasFeature(int feature, bool *hasFeature) { return false; }
bool FakeFrameGrabber::setFeature(int feature, double value) { return false; }
bool FakeFrameGrabber::getFeature(int feature, double *value) { return false; }
bool FakeFrameGrabber::setFeature(int feature, double  value1, double  value2) { return false; }
bool FakeFrameGrabber::getFeature(int feature, double *value1, double *value2) { return false; }
bool FakeFrameGrabber::hasOnOff(int feature, bool *HasOnOff) { return false; }
bool FakeFrameGrabber::setActive(int feature, bool onoff) { return false; }
bool FakeFrameGrabber::getActive(int feature, bool *isActive) { return false; }
bool FakeFrameGrabber::hasAuto(int feature, bool *hasAuto) { return false; }
bool FakeFrameGrabber::hasManual(int feature, bool *hasManual) { return false; }
bool FakeFrameGrabber::hasOnePush(int feature, bool *hasOnePush) { return false; }
bool FakeFrameGrabber::setMode(int feature, FeatureMode mode) { return false; }
bool FakeFrameGrabber::getMode(int feature, FeatureMode *mode) { return false; }
bool FakeFrameGrabber::setOnePush(int feature) { return false; }

void FakeFrameGrabber::printTime(unsigned char* pixbuf, size_t pixbuf_w, size_t pixbuf_h, size_t x, size_t y, char* s, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        const char* num_p = nullptr;
        switch (s[i]) {
            case '0': num_p = num[0]; break;
            case '1': num_p = num[1]; break;
            case '2': num_p = num[2]; break;
            case '3': num_p = num[3]; break;
            case '4': num_p = num[4]; break;
            case '5': num_p = num[5]; break;
            case '6': num_p = num[6]; break;
            case '7': num_p = num[7]; break;
            case '8': num_p = num[8]; break;
            case '9': num_p = num[9]; break;
            case ' ': num_p = num[10]; break;
            case '.': num_p = num[11]; break;
            default: num_p = num[10]; break;
        }

        for (size_t yi = 0; yi < num_height; yi++)
            for (size_t xi = 0; xi < num_width; xi++) {
                size_t ii = yi * num_width + xi;
                if (num_p[ii] == '*') {
                    for (size_t r = yi * num_height; r < yi*num_height + num_height; r++) {
                        size_t off = i * (num_height + 20);
                        for (size_t c = xi * num_height + off; c < xi*num_height + num_height + off; c++) {
                            if (c >= pixbuf_h || r >= pixbuf_w) {
                                //avoid drawing out of the image memory
                                return;
                            }
                            unsigned char *pixel = pixbuf;
                            size_t offset = c * sizeof(yarp::sig::PixelRgb) + r * (pixbuf_w * sizeof(yarp::sig::PixelRgb));
                            pixel = pixel + offset;
                            pixel[0] = 0;
                            pixel[1] = 0;
                            pixel[2] = 255;
                        }
                    }
                }
            }
    }
}

void FakeFrameGrabber::createTestImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                       double& timestamp) {
    // to test IPreciselyTimed, make timestamps be mysteriously NNN.NNN42
    double t = Time::now();
    t -= (((t*1000) - static_cast<int64_t>(t*1000)) / 1000);
    t += 0.00042;
    timestamp = t;
    image.resize(w,h);

    switch (mode) {
    case VOCAB_TIMETEXT:
        {
            if (have_bg) {
                image.copy(background);
            } else {
                image.zero();
            }
            char txtbuf[50];
            static const double start_time = t;
            double time = t - start_time;
            std::snprintf(txtbuf, 50, "%.3f", time);
            int len = strlen(txtbuf);
            if (len < 20)
            {
                printTime((unsigned char*)image.getRawImage(), image.width(), image.height(), 0, 0, txtbuf, len);
            }
        }
        break;
    case VOCAB_BALL:
        {
            if (have_bg) {
                image.copy(background);
            } else {
                image.zero();
            }
            addCircle(image,PixelRgb{0,255,0},bx,by,15);
            addCircle(image,PixelRgb{0,255,255},bx,by,8);
            if (ct%5!=0) {
                rnd *= 65537;
                rnd += 17;
                int delta_x = (rnd % 5) - 2;
                bx += delta_x;
                rnd *= 65537;
                rnd += 17;
                int delta_y = (rnd % 5) - 2;
                by += delta_y;
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
            size_t ww = image.width();
            size_t hh = image.height();
            if (ww>1&&hh>1) {
                for (size_t x=0; x<ww; x++) {
                    for (size_t y=0; y<hh; y++) {
                        double xx = ((double)x)/(ww-1);
                        double yy = ((double)y)/(hh-1);
                        bool act = (y==ct);
                        auto r = static_cast<unsigned char>(0.5 + 255 * xx);
                        auto g = static_cast<unsigned char>(0.5 + 255 * yy);
                        auto b = static_cast<unsigned char>(act * 255);
                        image.pixel(x, y) = PixelRgb{r, g, b};
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
            yCDebug(FAKEFRAMEGRABBER) << "size 100, 100";
            image.resize(100,100);
        }
        else if (count == 200)
        {
            yCDebug(FAKEFRAMEGRABBER) << "size 200, 100";
            image.resize(200, 100);
        }
        else if (count == 300)
        {
            yCDebug(FAKEFRAMEGRABBER) << "size 300, 50";
            image.resize(300, 50);
            count = 0;
        }

        size_t ww = w = image.width();
        size_t hh = h = image.height();
        if (ww>1 && hh>1) {
            for (size_t x = 0; x<ww; x++) {
                for (size_t y = 0; y<hh; y++) {
                    double xx = ((double)x) / (ww - 1);
                    double yy = ((double)y) / (hh - 1);
                    bool act = (y == ct);
                    auto r = static_cast<unsigned char>(0.5 + 255 * xx);
                    auto g = static_cast<unsigned char>(0.5 + 255 * yy);
                    auto b = static_cast<unsigned char>(act * 255);
                    image.pixel(x, y) = PixelRgb{r, g, b};
                }
            }
        }
    }
    break;
    case VOCAB_LINE:
    default:
        {
            if (have_bg) {
                image.copy(background);
            } else {
                image.zero();
            }
            for (size_t i=0; i<image.width(); i++) {
                image.pixel(i,ct).r = 255;
            }
        }
        break;
    case VOCAB_RAND:
        {
            static unsigned char r = 128;
            static unsigned char g = 128;
            static unsigned char b = 128;

            size_t ww = image.width();
            size_t hh = image.height();

            if (ww>1&&hh>1) {
                for (size_t x=0; x<ww; x++) {
                    for (size_t y=0; y<hh; y++) {
                        r += udist(randengine);
                        g += udist(randengine);
                        b += udist(randengine);
                        image.pixel(x,y) = PixelRgb{r,g,b};
                    }
                }
            }
        }
        break;
    case VOCAB_NONE:
        {
            if (have_bg) {
                image.copy(background);
            } else {
                image.zero();
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

    if (add_noise) {
        static const double nsr = 1.0 - snr;
        for (size_t x = 0; x < image.width(); ++x) {
            for (size_t y = 0; y < image.height(); ++y) {
                auto rand = ucdist(randengine);
                image.pixel(x,y) = PixelRgb {
                    static_cast<unsigned char>(image.pixel(x,y).r * snr + (rand * nsr * 255)),
                    static_cast<unsigned char>(image.pixel(x,y).g * snr + (rand * nsr * 255)),
                    static_cast<unsigned char>(image.pixel(x,y).b * snr + (rand * nsr * 255))
                };
            }
        }
    }

    if (add_timestamp) {
        char ttxt[50];
        std::snprintf(ttxt, 50, "%021.10f", timestamp);
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

    image.setTopIsLowIndex(topIsLow);
}



// From iCub staticgrabber device.
// DF2 bayer sequence.
// -- in staticgrabber: first row GBGBGB, second row RGRGRG.
// -- changed here to:  first row GRGRGR, second row BGBGBG.
bool FakeFrameGrabber::makeSimpleBayer(
        ImageOf<PixelRgb>& img,
        ImageOf<PixelMono>& bayer) {

    bayer.resize(img.width(), img.height());

    const size_t w = img.width();
    const size_t h = img.height();

    size_t i, j;
    for (i = 0; i < h; i++) {
        auto* row = (PixelRgb *)img.getRow(i);
        auto* rd = (PixelMono *)bayer.getRow(i);

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

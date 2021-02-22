/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_FAKEFRAMEGRABBER_FAKEFRAMEGRABBER_H
#define YARP_FAKEFRAMEGRABBER_FAKEFRAMEGRABBER_H


#include <yarp/sig/ImageFile.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/AudioVisualInterfaces.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Log.h>
#include <yarp/os/Value.h>
#include <yarp/dev/IVisualParams.h>

#include <cstdio>
#include <random>

/**
 * @ingroup dev_impl_media
 *
 * A fake camera for testing.
 * Implements the IFrameGrabberImage and IFrameGrabberControls
 * interfaces.
 */
class FakeFrameGrabber :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IFrameGrabberImage,
        public yarp::dev::IFrameGrabberImageRaw,
        public yarp::dev::IFrameGrabberControls,
        public yarp::dev::IPreciselyTimed,
        public yarp::dev::IAudioVisualStream,
        public yarp::dev::IRgbVisualParams
{
public:
    FakeFrameGrabber() = default;
    FakeFrameGrabber(const FakeFrameGrabber&) = delete;
    FakeFrameGrabber(FakeFrameGrabber&&) = delete;
    FakeFrameGrabber& operator=(const FakeFrameGrabber&) = delete;
    FakeFrameGrabber& operator=(FakeFrameGrabber&&) = delete;
    ~FakeFrameGrabber() override = default;

    bool close() override;

    /**
     * Configure with a set of options. These are:
     * <TABLE>
     * <TR><TD> width </TD><TD> Width of image (default 128). </TD></TR>
     * <TR><TD> height </TD><TD> Height of image (default 128). </TD></TR>
     * <TR><TD> freq </TD><TD> Frequency in Hz to generate images (default 20Hz). </TD></TR>
     * <TR><TD> period </TD><TD> Inverse of freq - only set one of these. </TD></TR>
     * <TR><TD> mode </TD><TD> Can be [line] (default), [ball], [grid], [rand], [none]. </TD></TR>
     * <TR><TD> src </TD><TD> Image file to read from (default: none). </TD></TR>
     * <TR><TD> bayer </TD><TD> Emit a bayer image. </TD></TR>
     * <TR><TD> mono </TD><TD> Emit a monochrome image. </TD></TR>
     * </TABLE>
     *
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    bool open(yarp::os::Searchable& config) override;

    void timing();

    int height() const override;

    int width() const override;
    //IRgbVisualParams
    int getRgbHeight() override;

    int getRgbWidth() override;

    bool getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig>& configurations) override;

    bool getRgbResolution(int& width, int& height) override;

    bool setRgbResolution(int width, int height) override;

    bool getRgbFOV(double& horizontalFov, double& verticalFov) override;

    bool setRgbFOV(double horizontalFov, double verticalFov) override;

    bool getRgbIntrinsicParam(yarp::os::Property& intrinsic) override;

    bool getRgbMirroring(bool& mirror) override;

    bool setRgbMirroring(bool mirror) override;
    //
    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override;

    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) override;

    yarp::os::Stamp getLastInputStamp() override;

    bool hasAudio() override;

    bool hasVideo() override;

    bool hasRawVideo() override;

    bool getCameraDescription(CameraDescriptor *camera) override;

    bool hasFeature(int feature, bool *hasFeature) override;

    bool setFeature(int feature, double value) override;

    bool getFeature(int feature, double *value) override;

    bool setFeature(int feature, double  value1, double  value2) override;

    bool getFeature(int feature, double *value1, double *value2) override;

    bool hasOnOff(int feature, bool *HasOnOff) override;

    bool setActive(int feature, bool onoff) override;

    bool getActive(int feature, bool *isActive) override;

    bool hasAuto(int feature, bool *hasAuto) override;

    bool hasManual(int feature, bool *hasManual) override;

    bool hasOnePush(int feature, bool *hasOnePush) override;

    bool setMode(int feature, FeatureMode mode) override;

    bool getMode(int feature, FeatureMode *mode) override;

    bool setOnePush(int feature) override;

private:
    static constexpr size_t default_w = 128;
    static constexpr size_t default_h = 128;
    static constexpr size_t default_freq = 30;

    size_t ct{0};
    size_t bx{0};
    size_t by{0};
    size_t w{default_w};
    size_t h{default_h};
    unsigned long rnd{0};
    double freq{default_freq};
    double period{1/freq};
    double first{0};
    double horizontalFov{0.0};
    double verticalFov{0.0};
    double prev{0};
    int mode{0};
    bool use_bayer{false};
    bool use_mono{false};
    bool mirror{false};
    yarp::os::Property intrinsic;
    yarp::sig::VectorOf<yarp::dev::CameraConfig> configurations;

    std::random_device rnddev;
    std::default_random_engine randengine{rnddev()};
    std::uniform_int_distribution<int> udist{-1, 1};

    yarp::sig::ImageOf<yarp::sig::PixelRgb> background;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> rgb_image;
    yarp::os::Stamp stamp;

    void createTestImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

    bool makeSimpleBayer(yarp::sig::ImageOf<yarp::sig::PixelRgb>& src,
                         yarp::sig::ImageOf<yarp::sig::PixelMono>& bayer);

    void printTime(unsigned char* pixbuf, size_t pixbuf_w, size_t pixbuf_h, size_t x, size_t y, char* s, size_t size);
};


class TestFrameGrabber : public FakeFrameGrabber
{
public:
    bool open(yarp::os::Searchable& config) override;
};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_fakeFrameGrabber fakeFrameGrabber

 A fake framegrabber, see yarp::dev::FakeFrameGrabber.

*/


#endif // YARP_FAKEFRAMEGRABBER_FAKEFRAMEGRABBER_H

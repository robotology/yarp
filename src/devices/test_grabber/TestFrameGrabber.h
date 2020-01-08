/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_TESTFRAMEGRABBER_H
#define YARP_DEV_TESTFRAMEGRABBER_H

#include <cstdio>

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

constexpr yarp::conf::vocab32_t VOCAB_LINE = yarp::os::createVocab('l','i','n','e');

/**
 * @ingroup dev_impl_media
 *
 * A fake camera for testing.
 * Implements the IFrameGrabberImage and IFrameGrabberControls
 * interfaces.
 */
class TestFrameGrabber :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IFrameGrabberImage,
        public yarp::dev::IFrameGrabberImageRaw,
        public yarp::dev::IFrameGrabberControls,
        public yarp::dev::IPreciselyTimed,
        public yarp::dev::IAudioVisualStream,
        public yarp::dev::IRgbVisualParams
{
private:
    size_t ct;
    size_t bx, by;
    size_t w, h;
    unsigned long rnd;
    double freq;
    double period;
    double first;
    double horizontalFov,verticalFov;
    double prev;
    int mode;
    bool use_bayer;
    bool use_mono;
    bool mirror;
    yarp::os::Property intrinsic;
    yarp::sig::VectorOf<yarp::dev::CameraConfig> configurations;

public:
    /**
     * Constructor.
     */
    TestFrameGrabber();

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
    yarp::sig::ImageOf<yarp::sig::PixelRgb> background, rgb_image;
    yarp::os::Stamp stamp;
    void createTestImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

    bool makeSimpleBayer(yarp::sig::ImageOf<yarp::sig::PixelRgb>& src,
                         yarp::sig::ImageOf<yarp::sig::PixelMono>& bayer);

    void printTime(unsigned char* pixbuf, int pixbuf_w, int pixbuf_h, int x, int y, char* s, int size);
    struct txtnum_type
    {
        char data[16];
    };
    txtnum_type num[12];
    double start_time;
};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_test_grabber test_grabber

 A fake framegrabber, see yarp::dev::TestFrameGrabber.

*/


#endif // YARP_DEV_TESTFRAMEGRABBER_H

/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/AudioVisualInterfaces.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Log.h>
#include <yarp/os/Value.h>
#include <yarp/dev/IVisualParams.h>

constexpr yarp::conf::vocab32_t VOCAB_LINE = yarp::os::createVocab('l','i','n','e');

namespace yarp {
    namespace dev {
        class TestFrameGrabber;
    }
}

/**
 * @ingroup dev_impl_media
 *
 * A fake camera for testing.
 * Implements the IFrameGrabberImage and IFrameGrabberControls
 * interfaces.
 */
class yarp::dev::TestFrameGrabber : public DeviceDriver,
                                    public IFrameGrabberImage,
                                    public IFrameGrabberImageRaw,
                                    public IFrameGrabberControls,
                                    public IPreciselyTimed,
                                    public IAudioVisualStream,
                                    public IRgbVisualParams
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
    yarp::sig::VectorOf<CameraConfig> configurations;

public:
    /**
     * Constructor.
     */
    TestFrameGrabber();

    virtual bool close() override;

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
    virtual bool open(yarp::os::Searchable& config) override;

    void timing();

    virtual int height() const override;

    virtual int width() const override;
    //IRgbVisualParams
    virtual int getRgbHeight() override;

    virtual int getRgbWidth() override;

    virtual bool getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations) override;

    virtual bool getRgbResolution(int &width, int &height) override;

    virtual bool setRgbResolution(int width, int height) override;

    virtual bool getRgbFOV(double &horizontalFov, double &verticalFov) override;

    virtual bool setRgbFOV(double horizontalFov, double verticalFov) override;

    virtual bool getRgbIntrinsicParam(yarp::os::Property &intrinsic) override;

    virtual bool getRgbMirroring(bool &mirror) override;

    virtual bool setRgbMirroring(bool mirror) override;
    //
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override;

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) override;

    virtual yarp::os::Stamp getLastInputStamp() override;

    virtual bool hasAudio() override;

    virtual bool hasVideo() override;

    virtual bool hasRawVideo() override;

    virtual bool getCameraDescription(CameraDescriptor *camera) override;

    virtual bool hasFeature(int feature, bool *hasFeature) override;

    virtual bool setFeature(int feature, double value) override;

    virtual bool getFeature(int feature, double *value) override;

    virtual bool setFeature(int feature, double  value1, double  value2) override;

    virtual bool getFeature(int feature, double *value1, double *value2) override;

    virtual bool hasOnOff(int feature, bool *HasOnOff) override;

    virtual bool setActive(int feature, bool onoff) override;

    virtual bool getActive(int feature, bool *isActive) override;

    virtual bool hasAuto(int feature, bool *hasAuto) override;

    virtual bool hasManual(int feature, bool *hasManual) override;

    virtual bool hasOnePush(int feature, bool *hasOnePush) override;

    virtual bool setMode(int feature, FeatureMode mode) override;

    virtual bool getMode(int feature, FeatureMode *mode) override;

    virtual bool setOnePush(int feature) override;

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

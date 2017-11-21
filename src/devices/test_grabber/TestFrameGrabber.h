/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

#define VOCAB_LINE VOCAB4('l','i','n','e')

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
    int ct;
    int bx, by;
    int w, h;
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

    virtual bool setBrightness(double v) override;

    virtual bool setExposure(double v) override;

    virtual bool setSharpness(double v) override;

    virtual bool setWhiteBalance(double blue, double red) override;

    virtual bool setHue(double v) override;

    virtual bool setSaturation(double v) override;

    virtual bool setGamma(double v) override;

    virtual bool setShutter(double v) override;

    virtual bool setGain(double v) override;

    virtual bool setIris(double v) override;

    virtual double getBrightness() override;

    virtual double getExposure() override;

    virtual double getSharpness() override;

    virtual bool getWhiteBalance(double &blue, double &red) override;

    virtual double getHue() override;

    virtual double getSaturation() override;

    virtual double getGamma() override;

    virtual double getShutter() override;

    virtual double getGain() override;

    virtual double getIris() override;

    virtual yarp::os::Stamp getLastInputStamp() override;

    virtual bool hasAudio() override;

    virtual bool hasVideo() override;

    virtual bool hasRawVideo() override;

private:
    yarp::sig::ImageOf<yarp::sig::PixelRgb> background, rgb_image;
    yarp::os::Stamp stamp;
    void createTestImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

    bool makeSimpleBayer(yarp::sig::ImageOf<yarp::sig::PixelRgb>& src,
                         yarp::sig::ImageOf<yarp::sig::PixelMono>& bayer);

};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_test_grabber test_grabber

 A fake framegrabber, see yarp::dev::TestFrameGrabber.

*/


#endif // YARP_DEV_TESTFRAMEGRABBER_H

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
    double period, freq;
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

    virtual bool close();

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
    virtual bool open(yarp::os::Searchable& config);

    void timing();

    virtual int height() const;

    virtual int width() const;
    //IRgbVisualParams
    virtual int getRgbHeight();

    virtual int getRgbWidth();

    virtual bool getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations);

    virtual bool getRgbResolution(int &width, int &height);

    virtual bool setRgbResolution(int width, int height);

    virtual bool getRgbFOV(double &horizontalFov, double &verticalFov);

    virtual bool setRgbFOV(double horizontalFov, double verticalFov);

    virtual bool getRgbIntrinsicParam(yarp::os::Property &intrinsic);

    virtual bool getRgbMirroring(bool &mirror);

    virtual bool setRgbMirroring(bool mirror);
    //
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image);

    virtual bool setBrightness(double v);

    virtual bool setExposure(double v);

    virtual bool setSharpness(double v);

    virtual bool setWhiteBalance(double blue, double red);

    virtual bool setHue(double v);

    virtual bool setSaturation(double v);

    virtual bool setGamma(double v);

    virtual bool setShutter(double v);

    virtual bool setGain(double v);

    virtual bool setIris(double v);

    virtual double getBrightness();

    virtual double getExposure();

    virtual double getSharpness();

    virtual bool getWhiteBalance(double &blue, double &red);

    virtual double getHue();

    virtual double getSaturation();

    virtual double getGamma();

    virtual double getShutter();

    virtual double getGain();

    virtual double getIris();

    virtual yarp::os::Stamp getLastInputStamp();

    virtual bool hasAudio();

    virtual bool hasVideo();

    virtual bool hasRawVideo();

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

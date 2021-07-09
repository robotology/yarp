/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>

#include <yarp/sig/all.h>

#include <yarp/dev/all.h>

class FakeFrameGrabber :
        public yarp::dev::IFrameGrabberImage,
        public yarp::dev::IFrameGrabberControls,
        public yarp::dev::DeviceDriver
{
private:
    int w, h;
    int ct;
    yarp::sig::PixelRgb circlePixel;
    double brightness;

public:
    FakeFrameGrabber()
    {
        h = w = 0;
        ct = 0;
        circlePixel = yarp::sig::PixelRgb{255, 0, 0};
        brightness = 1;
    }

    bool open(int w, int h)
    {
        this->w = w;
        this->h = h;
        return w > 0 && h > 0;
    }

    bool open(yarp::os::Searchable& config) override
    {
        // extract width and height configuration, if present
        // otherwise use 128x128
        int desiredWidth = config.check("w", yarp::os::Value(128)).asInt32();
        int desiredHeight = config.check("h", yarp::os::Value(128)).asInt32();
        return open(desiredWidth, desiredHeight);
    }

    bool close() override
    {
        return true; // easy
    }

    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override
    {
        yarp::os::Time::delay(0.1); // simulate waiting for hardware to report
        image.resize(w, h);
        image.zero();
        yarp::sig::draw::addCrossHair(image, circlePixel, ct, h / 2, h / 8);
        ct = (ct + 4) % w;
        return true;
    }

    int height() const override
    {
        return h;
    }

    int width() const override
    {
        return w;
    }

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
    bool setBrightness(double v) override
    {
        if (v > 1) {
            v = 1;
        }
        if (v < 0) {
            v = 0;
        }
        circlePixel = yarp::sig::PixelRgb{static_cast<unsigned char>(255 * v), 0, 0};
        brightness = v;
        return true;
    }

    bool setShutter(double v) override
    {
        YARP_UNUSED(v);
        return false;
    }

    bool setGain(double v) override
    {
        YARP_UNUSED(v);
        return false;
    }

    bool setWhiteBalance(double red, double green) override
    {
        YARP_UNUSED(red);
        YARP_UNUSED(green);
        return false;
    }

    bool getWhiteBalance(double& red, double& green)
        override
    {
        red = 0;
        green = 0;
        return false;
    }

    double getBrightness() override
    {
        return brightness;
    }

    double getShutter() override
    {
        return 0;
    }

    double getGain() override
    {
        return 0;
    }

    bool setExposure(double v) override
    {
        YARP_UNUSED(v);
        return false;
    }

    bool setSharpness(double v) override
    {
        YARP_UNUSED(v);
        return false;
    }

    bool setHue(double v) override
    {
        YARP_UNUSED(v);
        return false;
    }

    bool setSaturation(double v) override
    {
        YARP_UNUSED(v);
        return false;
    }

    bool setGamma(double v) override
    {
        YARP_UNUSED(v);
        return false;
    }

    bool setIris(double v) override
    {
        YARP_UNUSED(v);
        return false;
    }

    double getExposure() override
    {
        return 0.0;
    }

    double getSharpness() override
    {
        return 0.0;
    }

    double getHue() override
    {
        return 0.0;
    }

    double getSaturation() override
    {
        return 0.0;
    }

    double getGamma() override
    {
        return 0.0;
    }

    double getIris() override
    {
        return 0.0;
    }
#endif

    bool getCameraDescription(CameraDescriptor* camera) override
    {
        YARP_UNUSED(camera);
        return false;
    }

    bool hasFeature(int feature, bool* hasFeature) override
    {
        YARP_UNUSED(feature);
        YARP_UNUSED(hasFeature);
        return false;
    }

    bool setFeature(int feature, double value) override
    {
        YARP_UNUSED(feature);
        YARP_UNUSED(value);
        return false;
    }

    bool getFeature(int feature, double* value) override
    {
        YARP_UNUSED(feature);
        YARP_UNUSED(value);
        return false;
    }

    bool setFeature(int feature, double value1, double value2) override
    {
        YARP_UNUSED(feature);
        YARP_UNUSED(value1);
        YARP_UNUSED(value2);
        return false;
    }

    bool getFeature(int feature, double* value1, double* value2) override
    {
        YARP_UNUSED(feature);
        YARP_UNUSED(value1);
        YARP_UNUSED(value2);
        return false;
    }

    bool hasOnOff(int feature, bool* HasOnOff) override
    {
        YARP_UNUSED(feature);
        YARP_UNUSED(HasOnOff);
        return false;
    }

    bool setActive(int feature, bool onoff) override
    {
        YARP_UNUSED(feature);
        YARP_UNUSED(onoff);
        return false;
    }

    bool getActive(int feature, bool* isActive) override
    {
        YARP_UNUSED(feature);
        YARP_UNUSED(isActive);
        return false;
    }

    bool hasAuto(int feature, bool* hasAuto) override
    {
        YARP_UNUSED(feature);
        YARP_UNUSED(hasAuto);
        return false;
    }

    bool hasManual(int feature, bool* hasManual) override
    {
        YARP_UNUSED(feature);
        YARP_UNUSED(hasManual);
        return false;
    }

    bool hasOnePush(int feature, bool* hasOnePush) override
    {
        YARP_UNUSED(feature);
        YARP_UNUSED(hasOnePush);
        return false;
    }

    bool setMode(int feature, FeatureMode mode) override
    {
        YARP_UNUSED(feature);
        YARP_UNUSED(mode);
        return false;
    }

    bool getMode(int feature, FeatureMode* mode) override
    {
        YARP_UNUSED(feature);
        YARP_UNUSED(mode);
        return false;
    }

    bool setOnePush(int feature) override
    {
        YARP_UNUSED(feature);
        return false;
    }
};

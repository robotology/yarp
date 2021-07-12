/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FRAMEGRABBERCROPPER_FRAMEGRABBERCROPPER_H
#define YARP_FRAMEGRABBERCROPPER_FRAMEGRABBERCROPPER_H

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IFrameGrabberControlsDC1394.h>
#include <yarp/dev/IRgbVisualParams.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/IWrapper.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <limits>

template <typename ImageType>
class FrameGrabberCropperOf :
        public yarp::dev::IFrameGrabberOf<ImageType>
{
protected:
    yarp::dev::IFrameGrabberOf<ImageType>* iFrameGrabberOf {nullptr};
    static constexpr cropType_id_t cropType {YARP_CROP_RECT};
    yarp::sig::VectorOf<std::pair<int, int>> vertices;

public:
    ~FrameGrabberCropperOf() override = default;

    bool getImage(ImageType& image) override
    {
        if (iFrameGrabberOf != nullptr) {
            return iFrameGrabberOf->getImageCrop(cropType, vertices, image);
        }
        return false;
    }

    int height() const override
    {
        if (vertices.size() == 2) {
            return vertices[1].second - vertices[0].second + 1;
        }
        return 0;
    }

    int width() const override
    {
        if (vertices.size() == 2) {
            return vertices[1].first - vertices[0].first + 1;
        }
        return 0;
    }
};


class FrameGrabberCropper :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IWrapper,
        public yarp::dev::IMultipleWrapper,
        public FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>,
        public FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>,
        public yarp::dev::IFrameGrabberControls,
        public yarp::dev::IFrameGrabberControlsDC1394,
        public yarp::dev::IRgbVisualParams,
        public yarp::dev::IPreciselyTimed
{
    yarp::dev::PolyDriver* subdevice{nullptr};
    bool subdeviceOwned{false};

    yarp::dev::IFrameGrabberControls* iFrameGrabberControls{nullptr};
    yarp::dev::IFrameGrabberControlsDC1394* iFrameGrabberControlsDC1394{nullptr};
    yarp::dev::IRgbVisualParams* iRgbVisualParams{nullptr};
    yarp::dev::IPreciselyTimed* iPreciselyTimed{nullptr};

    bool forwardRgbVisualParams{false};

public:
    FrameGrabberCropper() = default;
    FrameGrabberCropper(const FrameGrabberCropper&) = delete;
    FrameGrabberCropper(FrameGrabberCropper&&) = delete;
    FrameGrabberCropper& operator=(const FrameGrabberCropper&) = delete;
    FrameGrabberCropper& operator=(FrameGrabberCropper&&) = delete;
    ~FrameGrabberCropper() override;

    // yarp::dev::DeviceDriver
    bool close() override;
    bool open(yarp::os::Searchable& config) override;

    // yarp::dev::IWrapper
    bool attach(yarp::dev::PolyDriver* poly) override;
    bool detach() override;

    // yarp::dev::IMultipleWrapper
    bool attachAll(const yarp::dev::PolyDriverList& device2attach) override;
    bool detachAll() override;


    // yarp::dev::IFrameGrabberControls
    bool getCameraDescription(CameraDescriptor* camera) override
    {
        if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->getCameraDescription(camera);
    }

    bool hasFeature(int feature, bool* hasFeature) override
    {
        if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->hasFeature(feature, hasFeature);
    }

    bool setFeature(int feature, double value) override
    {    if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->setFeature(feature, value);
    }

    bool getFeature(int feature, double* value) override
    {
        if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->getFeature(feature, value);
    }

    bool setFeature(int feature, double value1, double value2) override
    {
        if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->setFeature(feature, value1, value2);
    }

    bool getFeature(int feature, double* value1, double* value2) override
    {
        if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->getFeature(feature, value1, value2);
    }

    bool hasOnOff(int feature, bool* HasOnOff) override
    {
        if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->hasOnOff(feature, HasOnOff);
    }

    bool setActive(int feature, bool onoff) override
    {
        if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->setActive(feature, onoff);
    }

    bool getActive(int feature, bool* isActive) override
    {
        if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->getActive(feature, isActive);
    }

    bool hasAuto(int feature, bool* hasAuto) override
    {
        if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->hasAuto(feature, hasAuto);
    }

    bool hasManual(int feature, bool* hasManual) override
    {
        if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->hasManual(feature, hasManual);
    }

    bool hasOnePush(int feature, bool* hasOnePush) override
    {
        if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->hasOnePush(feature, hasOnePush);
    }

    bool setMode(int feature, FeatureMode mode) override
    {
        if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->setMode(feature, mode);
    }

    bool getMode(int feature, FeatureMode* mode) override
    {
        if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->getMode(feature, mode);
    }

    bool setOnePush(int feature) override
    {
        if (!iFrameGrabberControls) {
            return false;
        }
        return iFrameGrabberControls->setOnePush(feature);
    }


    // yarp::dev::IFrameGrabberControlsDC1394
    unsigned int getVideoModeMaskDC1394() override
    {
        if (!iFrameGrabberControlsDC1394) {
            return 0;
        }
        return iFrameGrabberControlsDC1394->getVideoModeMaskDC1394();
    }

    unsigned int getVideoModeDC1394() override
    {
        if (!iFrameGrabberControlsDC1394) {
            return 0;
        }
        return iFrameGrabberControlsDC1394->getVideoModeMaskDC1394();
    }

    bool setVideoModeDC1394(int video_mode) override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->setVideoModeDC1394(video_mode);
    }

    unsigned int getFPSMaskDC1394() override
    {
        if (!iFrameGrabberControlsDC1394) {
            return 0;
        }
        return iFrameGrabberControlsDC1394->getFPSMaskDC1394();
    }

    unsigned int getFPSDC1394() override

    {
        if (!iFrameGrabberControlsDC1394) {
            return 0;
        }
        return iFrameGrabberControlsDC1394->getFPSDC1394();
    }

    bool setFPSDC1394(int fps) override

    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->setFPSDC1394(fps);
    }

    unsigned int getISOSpeedDC1394() override
    {
        if (!iFrameGrabberControlsDC1394) {
            return 0;
        }
        return iFrameGrabberControlsDC1394->getISOSpeedDC1394();
    }

    bool setISOSpeedDC1394(int speed) override

    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->setISOSpeedDC1394(speed);
    }

    unsigned int getColorCodingMaskDC1394(unsigned int video_mode) override
    {
        if (!iFrameGrabberControlsDC1394) {
            return 0;
        }
        return iFrameGrabberControlsDC1394->getColorCodingMaskDC1394(video_mode);
    }

    unsigned int getColorCodingDC1394() override
    {
        if (!iFrameGrabberControlsDC1394) {
            return 0;
        }
        return iFrameGrabberControlsDC1394->getColorCodingDC1394();
    }

    bool setColorCodingDC1394(int coding) override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->setColorCodingDC1394(coding);
    }

    bool getFormat7MaxWindowDC1394(unsigned int& xdim,
                                   unsigned int& ydim,
                                   unsigned int& xstep,
                                   unsigned int& ystep,
                                   unsigned int& xoffstep,
                                   unsigned int& yoffstep) override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->getFormat7MaxWindowDC1394(xdim, ydim, xstep, ystep, xoffstep, yoffstep);
    }

    bool getFormat7WindowDC1394(unsigned int& xdim, unsigned int& ydim, int& x0, int& y0) override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->getFormat7WindowDC1394(xdim, ydim, x0, y0);
    }

    bool setFormat7WindowDC1394(unsigned int xdim, unsigned int ydim, int x0, int y0) override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->setFormat7WindowDC1394(xdim, ydim, x0, y0);
    }

    bool setOperationModeDC1394(bool b1394b) override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->setOperationModeDC1394(b1394b);
    }

    bool getOperationModeDC1394() override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->getOperationModeDC1394();
    }

    bool setTransmissionDC1394(bool bTxON) override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->setTransmissionDC1394(bTxON);
    }

    bool getTransmissionDC1394() override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->getTransmissionDC1394();
    }

    bool setBroadcastDC1394(bool onoff) override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->setBroadcastDC1394(onoff);
    }

    bool setDefaultsDC1394() override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->setDefaultsDC1394();
    }

    bool setResetDC1394() override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->setResetDC1394();
    }

    bool setPowerDC1394(bool onoff) override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->setPowerDC1394(onoff);
    }

    bool setCaptureDC1394(bool bON) override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->setCaptureDC1394(bON);
    }

    unsigned int getBytesPerPacketDC1394() override
    {
        if (!iFrameGrabberControlsDC1394) {
            return 0;
        }
        return iFrameGrabberControlsDC1394->getBytesPerPacketDC1394();
    }

    bool setBytesPerPacketDC1394(unsigned int bpp) override
    {
        if (!iFrameGrabberControlsDC1394) {
            return false;
        }
        return iFrameGrabberControlsDC1394->setBytesPerPacketDC1394(bpp);
    }


    // yarp::dev::IRgbVisualParams
    int getRgbHeight() override
    {
        return FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::height();
    }

    int getRgbWidth() override
    {
        return FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::width();
    }

    bool setRgbResolution(int width, int height) override
    {
        if (!iRgbVisualParams || !forwardRgbVisualParams) {
            return false;
        }
        return iRgbVisualParams->setRgbResolution(width, height);
    }

    bool getRgbFOV(double& horizontalFov, double& verticalFov) override
    {
        if (!iRgbVisualParams || !forwardRgbVisualParams) {
            horizontalFov = std::numeric_limits<double>::quiet_NaN();
            verticalFov = std::numeric_limits<double>::quiet_NaN();
            return false;
        }
        return iRgbVisualParams->getRgbFOV(horizontalFov, verticalFov);
    }

    bool setRgbFOV(double horizontalFov, double verticalFov) override
    {
        if (!iRgbVisualParams || !forwardRgbVisualParams) {
            return false;
        }
        return iRgbVisualParams->setRgbFOV(horizontalFov, verticalFov);
    }

    bool getRgbIntrinsicParam(yarp::os::Property& intrinsic) override
    {
        if (!iRgbVisualParams || !forwardRgbVisualParams) {
            intrinsic.clear();
            return false;
        }
        return iRgbVisualParams->getRgbIntrinsicParam(intrinsic);
    }

    bool getRgbMirroring(bool& mirror) override
    {
        if (!iRgbVisualParams || !forwardRgbVisualParams) {
            mirror = false;
            return false;
        }
        return iRgbVisualParams->getRgbMirroring(mirror);

    }

    bool setRgbMirroring(bool mirror) override
    {
        if (!iRgbVisualParams || !forwardRgbVisualParams) {
            return false;
        }
        return iRgbVisualParams->setRgbMirroring(mirror);
    }


    // yarp::dev::IPreciselyTimed
    yarp::os::Stamp getLastInputStamp() override
    {
        if (!iPreciselyTimed) {
            return yarp::os::Stamp();
        }
        return iPreciselyTimed->getLastInputStamp();
    }
};

#endif // YARP_FRAMEGRABBERCROPPER_FRAMEGRABBERCROPPER_H

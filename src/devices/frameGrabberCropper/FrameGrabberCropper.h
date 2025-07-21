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
#include "FrameGrabberCropper_ParamsParser.h"

template <typename ImageType>
class FrameGrabberCropperOf :
        public yarp::dev::IFrameGrabberOf<ImageType>
{
protected:
    yarp::dev::IFrameGrabberOf<ImageType>* iFrameGrabberOf {nullptr};
    static constexpr yarp::dev::cropType_id_t cropType {yarp::dev::cropType_id_t::YARP_CROP_RECT};
    std::vector<yarp::dev::vertex_t> vertices;

public:
    ~FrameGrabberCropperOf() override = default;

    yarp::dev::ReturnValue getImage(ImageType& image) override
    {
        if (iFrameGrabberOf != nullptr) {
            return iFrameGrabberOf->getImageCrop(cropType, vertices, image);
        }
        return yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
    }

    int height() const override
    {
        if (vertices.size() == 2) {
            return vertices[1].y - vertices[0].y + 1;
        }
        return 0;
    }

    int width() const override
    {
        if (vertices.size() == 2) {
            return vertices[1].x - vertices[0].x + 1;
        }
        return 0;
    }
};

/**
 * @ingroup dev_impl_media
 *
 * \brief `FrameGrabberCropper`: An image frame grabber device which can also crop the frame.
 * In must be attached to another grabber.
 *
 * Parameters required by this device are shown in class: FrameGrabberCropper_ParamsParser
 */
class FrameGrabberCropper :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IWrapper,
        public yarp::dev::IMultipleWrapper,
        public FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>,
        public FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>,
        public yarp::dev::IFrameGrabberControls,
        public yarp::dev::IFrameGrabberControlsDC1394,
        public yarp::dev::IRgbVisualParams,
        public yarp::dev::IPreciselyTimed,
        public FrameGrabberCropper_ParamsParser
{
    yarp::dev::IFrameGrabberControls* iFrameGrabberControls{nullptr};
    yarp::dev::IFrameGrabberControlsDC1394* iFrameGrabberControlsDC1394{nullptr};
    yarp::dev::IRgbVisualParams* iRgbVisualParams{nullptr};
    yarp::dev::IPreciselyTimed* iPreciselyTimed{nullptr};

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
    yarp::dev::ReturnValue getCameraDescription(yarp::dev::CameraDescriptor& camera) override;
    yarp::dev::ReturnValue hasFeature(yarp::dev::cameraFeature_id_t feature, bool& hasFeature) override;
    yarp::dev::ReturnValue setFeature(yarp::dev::cameraFeature_id_t feature, double value) override;
    yarp::dev::ReturnValue getFeature(yarp::dev::cameraFeature_id_t feature, double& value) override;
    yarp::dev::ReturnValue setFeature(yarp::dev::cameraFeature_id_t feature, double value1, double value2) override;
    yarp::dev::ReturnValue getFeature(yarp::dev::cameraFeature_id_t feature, double& value1, double& value2) override;
    yarp::dev::ReturnValue hasOnOff(yarp::dev::cameraFeature_id_t feature, bool& HasOnOff) override;
    yarp::dev::ReturnValue setActive(yarp::dev::cameraFeature_id_t feature, bool onoff) override;
    yarp::dev::ReturnValue getActive(yarp::dev::cameraFeature_id_t feature, bool& isActive) override;
    yarp::dev::ReturnValue hasAuto(yarp::dev::cameraFeature_id_t feature, bool& hasAuto) override;
    yarp::dev::ReturnValue hasManual(yarp::dev::cameraFeature_id_t feature, bool& hasManual) override;
    yarp::dev::ReturnValue hasOnePush(yarp::dev::cameraFeature_id_t feature, bool& hasOnePush) override;
    yarp::dev::ReturnValue setMode(yarp::dev::cameraFeature_id_t feature, yarp::dev::FeatureMode mode) override;
    yarp::dev::ReturnValue getMode(yarp::dev::cameraFeature_id_t feature, yarp::dev::FeatureMode& mode) override;
    yarp::dev::ReturnValue setOnePush(yarp::dev::cameraFeature_id_t feature) override;

    // yarp::dev::IFrameGrabberControlsDC1394
    yarp::dev::ReturnValue getVideoModeMaskDC1394(unsigned int& val) override;
    yarp::dev::ReturnValue getVideoModeDC1394(unsigned int& val) override;
    yarp::dev::ReturnValue setVideoModeDC1394(int video_mode) override;
    yarp::dev::ReturnValue getFPSMaskDC1394(unsigned int& val) override;
    yarp::dev::ReturnValue getFPSDC1394(unsigned int& val) override;
    yarp::dev::ReturnValue setFPSDC1394(int fps) override;
    yarp::dev::ReturnValue getISOSpeedDC1394(unsigned int& val) override;
    yarp::dev::ReturnValue setISOSpeedDC1394(int speed) override;
    yarp::dev::ReturnValue getColorCodingMaskDC1394(unsigned int video_mode, unsigned int& val) override;
    yarp::dev::ReturnValue getColorCodingDC1394(unsigned int& val) override;
    yarp::dev::ReturnValue setColorCodingDC1394(int coding) override;
    yarp::dev::ReturnValue getFormat7MaxWindowDC1394(unsigned int& xdim,
                                   unsigned int& ydim,
                                   unsigned int& xstep,
                                   unsigned int& ystep,
                                   unsigned int& xoffstep,
                                   unsigned int& yoffstep) override;
    yarp::dev::ReturnValue getFormat7WindowDC1394(unsigned int& xdim, unsigned int& ydim, int& x0, int& y0) override;
    yarp::dev::ReturnValue setFormat7WindowDC1394(unsigned int xdim, unsigned int ydim, int x0, int y0) override;
    yarp::dev::ReturnValue setOperationModeDC1394(bool b1394b) override;
    yarp::dev::ReturnValue getOperationModeDC1394(bool& b1394b) override;
    yarp::dev::ReturnValue setTransmissionDC1394(bool bTxON) override;
    yarp::dev::ReturnValue getTransmissionDC1394(bool& bTxON) override;
    yarp::dev::ReturnValue setBroadcastDC1394(bool onoff) override;
    yarp::dev::ReturnValue setDefaultsDC1394() override;
    yarp::dev::ReturnValue setResetDC1394() override;
    yarp::dev::ReturnValue setPowerDC1394(bool onoff) override;
    yarp::dev::ReturnValue setCaptureDC1394(bool bON) override;
    yarp::dev::ReturnValue getBytesPerPacketDC1394(unsigned int& bpp) override;
    yarp::dev::ReturnValue setBytesPerPacketDC1394(unsigned int bpp) override;

    // yarp::dev::IRgbVisualParams
    int getRgbHeight() override;
    int getRgbWidth() override;
    yarp::dev::ReturnValue getRgbResolution(int& width, int& height) override;
    yarp::dev::ReturnValue setRgbResolution(int width, int height) override;
    yarp::dev::ReturnValue getRgbFOV(double& horizontalFov, double& verticalFov) override;
    yarp::dev::ReturnValue setRgbFOV(double horizontalFov, double verticalFov) override;
    yarp::dev::ReturnValue getRgbIntrinsicParam(yarp::os::Property& intrinsic) override;
    yarp::dev::ReturnValue getRgbSupportedConfigurations(std::vector<yarp::dev::CameraConfig>& cfgs) override;
    yarp::dev::ReturnValue getRgbMirroring(bool& mirror) override;
    yarp::dev::ReturnValue setRgbMirroring(bool mirror) override;

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

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FRAMEGRABBER_NWC_YARP_H
#define YARP_FRAMEGRABBER_NWC_YARP_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/Port.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/ReturnValue.h>

#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IFrameGrabberControlsDC1394.h>
#include <yarp/dev/IRgbVisualParams.h>

#include "FrameGrabberMsgs.h"
#include "StreamReceiver.h"

#include <mutex>
#include "FrameGrabber_nwc_yarp_ParamsParser.h"

template <typename ImageType>
class FrameGrabberOf_ForwarderWithStream:
        public yarp::dev::IFrameGrabberOf<ImageType>
{
public:
    FrameGrabberOf_ForwarderWithStream(FrameGrabberMsgs* thriftClient);
    ~FrameGrabberOf_ForwarderWithStream() override = default;

    // Re-implement the IFrameGrabberOf methods, to use the image received from
    // the streaming port when enabled, instead of calling the requesting them
    // using RPC calls
    int height() const override;
    int width() const override;
    yarp::dev::ReturnValue getImage(ImageType& image) override;
    yarp::dev::ReturnValue getImageCrop(yarp::dev::cropType_id_t cropType,
                      std::vector<yarp::dev::vertex_t> vertices,
                      ImageType& image) override;

    void setStreamReceiver(StreamReceiver* m_streamReceiver);

private:
    mutable std::mutex m_mutex;
    StreamReceiver* m_streamReceiver {nullptr};
    FrameGrabberMsgs* m_thriftClient {nullptr};
};


/**
 * @ingroup dev_impl_nwc_yarp
 *
 * \section frameGrabber_nwc_yarp
 *
 * \brief `frameGrabber_nwc_yarp`: The client of a frameGrabber_nws_yarp.
 *
 * Parameters required by this device are shown in class: FrameGrabber_nwc_yarp_ParamsParser
 */
class FrameGrabber_nwc_yarp :
        public yarp::dev::DeviceDriver,
        public FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelRgb>>,
        public FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelMono>>,
        public FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelFloat>>,
        public FrameGrabberOf_ForwarderWithStream<yarp::sig::FlexImage>,
        public yarp::dev::IFrameGrabberControls,
        public yarp::dev::IFrameGrabberControlsDC1394,
        public yarp::dev::IRgbVisualParams,
        public yarp::dev::IPreciselyTimed,
        public FrameGrabber_nwc_yarp_ParamsParser
{
public:
    FrameGrabber_nwc_yarp();
    FrameGrabber_nwc_yarp(const FrameGrabber_nwc_yarp&) = delete;
    FrameGrabber_nwc_yarp(FrameGrabber_nwc_yarp&&) = delete;
    FrameGrabber_nwc_yarp& operator=(const FrameGrabber_nwc_yarp&) = delete;
    FrameGrabber_nwc_yarp& operator=(FrameGrabber_nwc_yarp&&) = delete;
    ~FrameGrabber_nwc_yarp() override = default;

    //IRgbVisualParams
    int getRgbHeight() override;
    int getRgbWidth() override;
    yarp::dev::ReturnValue getRgbSupportedConfigurations(std::vector<yarp::dev::CameraConfig>& configurations) override;
    yarp::dev::ReturnValue getRgbResolution(int& width, int& height) override;
    yarp::dev::ReturnValue setRgbResolution(int width, int height) override;
    yarp::dev::ReturnValue getRgbFOV(double& horizontalFov, double& verticalFov) override;
    yarp::dev::ReturnValue setRgbFOV(double horizontalFov, double verticalFov) override;
    yarp::dev::ReturnValue getRgbIntrinsicParam(yarp::os::Property& intrinsic) override;
    yarp::dev::ReturnValue getRgbMirroring(bool& mirror) override;
    yarp::dev::ReturnValue setRgbMirroring(bool mirror) override;

    //IFrameGrabberControls
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

    // yarp::dev::DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // yarp::dev::IPreciselyTimed
    yarp::os::Stamp getLastInputStamp() override;

private:
    StreamReceiver   m_streamReceiver;
    yarp::os::Port   m_rpcPort;
    std::mutex       m_mutex;

    FrameGrabberMsgs m_frameGrabber_RPC;
};

#endif // YARP_FRAMEGRABBER_NWC_YARP_H

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameGrabber_nwc_yarp.h"

#include <yarp/os/Network.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/ReturnValue.h>

#include <yarp/sig/ImageUtils.h>

namespace {
YARP_LOG_COMPONENT(FRAMEGRABBER_NWC_YARP, "yarp.devices.frameGrabber_nwc_yarp")
} // namespace

using namespace yarp::dev;
using namespace yarp::sig;

// BEGIN FrameGrabberOf_ForwarderWithStream
template <typename ImageType>
FrameGrabberOf_ForwarderWithStream<ImageType>::FrameGrabberOf_ForwarderWithStream(FrameGrabberMsgs* thriftClient)
{
    m_thriftClient = thriftClient;
}

template <typename ImageType>
int FrameGrabberOf_ForwarderWithStream<ImageType>::height() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    // If not configured for receiving streams, do a RPC call via thrift
    if (!m_streamReceiver)
    {
        if (!m_thriftClient) {
            //return ReturnValue::return_code::return_value_error_generic;
            return 0;
        }
        auto ret = m_thriftClient->getHeightRPC();
        if (ret.ret) {
            return ret.val;
        }
        return 0;
    }
    else
    {
        return m_streamReceiver->lastHeight();
    }
}

template <typename ImageType>
int FrameGrabberOf_ForwarderWithStream<ImageType>::width() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    // If not configured for receiving streams, do a RPC call via thrift
    if (!m_streamReceiver)
    {
        if (!m_thriftClient)
        {
            //return ReturnValue::return_code::return_value_error_generic;
            return 0;
        }
        auto ret = m_thriftClient->getWidthRPC();
        if (ret.ret)
        {
            return ret.val;
        }
        return 0;
    }
    else
    {
        return m_streamReceiver->lastWidth();
    }
}

template <typename ImageType>
yarp::dev::ReturnValue FrameGrabberOf_ForwarderWithStream<ImageType>::getImage(ImageType& image)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    // If not configured for receiving streams, do an RPC call via thrift
    if (!m_streamReceiver)
    {
        if (!m_thriftClient)
        {
            return ReturnValue::return_code::return_value_error_generic;
        }
        auto ret = m_thriftClient->getImageRPC();
        if (ret.ret)
        {
            image.copy(ret.fImage);
        }
        return ret.ret;
    }

    bool b= m_streamReceiver->lastImage(image);
    if (!b) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return ReturnValue_ok;
}

template <typename ImageType>
yarp::dev::ReturnValue FrameGrabberOf_ForwarderWithStream<ImageType>::getImageCrop(cropType_id_t cropType,
                                                                                   std::vector<yarp::dev::vertex_t> vertices,
                                                                                   ImageType& image)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    // If not configured for receiving streams, do an RPC call via thrift
    if(!m_streamReceiver)
    {
        if (!m_thriftClient) {
            return ReturnValue::return_code::return_value_error_generic;
        }
        std::vector<yarp::dev::vertex_t> vv;
        vv.resize(vertices.size());
        for (size_t i = 0; i < vertices.size(); i++)
        {
            vv[i] = vertices[i];
        }
        auto ret = m_thriftClient->getImageCropRPC(cropType, vv);
        if (ret.ret) {
            image.copy(ret.fImage);
        }
        return ret.ret;
    }

    if (cropType == YARP_CROP_RECT) {
        if (vertices.size() != 2) {
            yCError(FRAMEGRABBER_NWC_YARP, "GetImageCrop failed: RECT mode requires 2 vertices");
            return ReturnValue::return_code::return_value_error_method_failed;
        }
        ImageType full;
        bool b = m_streamReceiver->lastImage(full);
        if (!b || full.width() == 0 || full.height() == 0)
        {
            yCError(FRAMEGRABBER_NWC_YARP, "GetImageCrop failed: No image received");
            return ReturnValue::return_code::return_value_error_not_ready;
        }

        std::pair<unsigned int, unsigned int> v0;
        v0.first = vertices[0].x;
        v0.second = vertices[0].y;
        std::pair<unsigned int, unsigned int> v1;
        v1.first = vertices[1].x;
        v1.second = vertices[1].y;
        if (!yarp::sig::utils::cropRect(full, v0, v1, image)) {
            yCError(FRAMEGRABBER_NWC_YARP, "GetImageCrop failed: utils::cropRect error: (%d, %d) (%d, %d)",
                vertices[0].x,
                vertices[0].y,
                vertices[1].x,
                vertices[1].y);
            return ReturnValue::return_code::return_value_error_method_failed;
        }
    }
    else if (cropType == YARP_CROP_LIST) {
        yCError(FRAMEGRABBER_NWC_YARP, "List type not yet implemented");
        return ReturnValue::return_code::return_value_error_not_implemented_by_device;
    }

    return ReturnValue_ok;
}


template <typename ImageType>
void FrameGrabberOf_ForwarderWithStream<ImageType>::setStreamReceiver(StreamReceiver* streamReceiver)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_streamReceiver = streamReceiver;
}
// END FrameGrabberOf_ForwarderWithStream

// BEGIN FrameGrabber_nwc_yarp
FrameGrabber_nwc_yarp::FrameGrabber_nwc_yarp() :
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelRgb>>(&m_frameGrabber_RPC),
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelMono>>(&m_frameGrabber_RPC),
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelFloat>>(&m_frameGrabber_RPC),
        FrameGrabberOf_ForwarderWithStream<yarp::sig::FlexImage>(&m_frameGrabber_RPC)
{
}


bool FrameGrabber_nwc_yarp::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) { return false; }

    if (!m_no_stream) {
        if (!m_streamReceiver.open(m_local, m_remote, m_carrier)) {
            return false;
        }
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::setStreamReceiver(&m_streamReceiver);
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelMono>>::setStreamReceiver(&m_streamReceiver);
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelFloat>>::setStreamReceiver(&m_streamReceiver);
        FrameGrabberOf_ForwarderWithStream<yarp::sig::FlexImage>::setStreamReceiver(&m_streamReceiver);
    }

    std::string rpc_local = m_local + "/rpc_client";
    std::string rpc_remote = m_remote + "/rpc";
    if (!m_rpcPort.open(rpc_local)) {
        yCError(FRAMEGRABBER_NWC_YARP) << "Failed to open " << rpc_local << "port.";
    }

    if (!m_remote.empty()) {
        yCInfo(FRAMEGRABBER_NWC_YARP) << "Connecting" << m_rpcPort.getName() << "to" << rpc_remote;
        if (!yarp::os::NetworkBase::connect(m_rpcPort.getName(), rpc_remote)) {
            yCError(FRAMEGRABBER_NWC_YARP) << "Failed to connect" << m_rpcPort.getName() << "to" << rpc_remote;
            return false;
        }
    } else {
        yCInfo(FRAMEGRABBER_NWC_YARP) << "No remote specified. Waiting for connection";
    }

    ///attach the message server
    if (!m_frameGrabber_RPC.yarp().attachAsClient(m_rpcPort))
    {
        yCError(FRAMEGRABBER_NWC_YARP, "Error! Cannot attach the port as a client");
        m_rpcPort.close();
        return false;
    }

    // Check the protocol version
    if (!m_frameGrabber_RPC.checkProtocolVersion()) {
        return false;
    }

    return true;
}

bool FrameGrabber_nwc_yarp::close()
{
    m_rpcPort.interrupt();
    m_rpcPort.close();

    m_streamReceiver.close();

    return true;
}


yarp::os::Stamp FrameGrabber_nwc_yarp::getLastInputStamp()
{
    return m_streamReceiver.lastStamp();
}


/*
*  IRgbVisualParams interface. Look at IVisualParams.h for documentation
*/
int FrameGrabber_nwc_yarp::getRgbHeight()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getRgbHeightRPC();
    return r.height;
}

int FrameGrabber_nwc_yarp::getRgbWidth()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getRgbWidthRPC();
    return r.width;
}

ReturnValue FrameGrabber_nwc_yarp::getRgbSupportedConfigurations(std::vector<CameraConfig> &configurations)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getRgbSupportedConfigurationsRPC();
    configurations = r.configuration;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::getRgbResolution(int &width, int &height)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getRgbResolutionRPC();
    height = r.height;
    width = r.width;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setRgbResolution(int width, int height)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setRgbResolutionRPC(width, height);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    yarp::os::Value val("RGB");
    auto r = m_frameGrabber_RPC.getRgbFOVRPC();
    horizontalFov = r.horizontalFov;
    verticalFov = r.verticalFOV;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setRgbFOV(double horizontalFov, double verticalFov)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setRgbFOVRPC(horizontalFov, verticalFov);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getRgbIntrinsicParam(yarp::os::Property &intrinsic)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r =  m_frameGrabber_RPC.getRgbIntrinsicParamRPC();
    intrinsic = r.params;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::getRgbMirroring(bool& mirror)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r= m_frameGrabber_RPC.getRgbMirroringRPC();
    mirror = r.mirror;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setRgbMirroring(bool mirror)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setRgbMirroringRPC(mirror);
    return r;
}

/*
* IFrameGrabberControls specific interface methods
*/
ReturnValue FrameGrabber_nwc_yarp::getCameraDescription(yarp::dev::CameraDescriptor& camera)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getCameraDescriptionRPC();
    camera.busType = r.camera.busType;
    camera.deviceDescription = r.camera.deviceDescription;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::hasFeature(cameraFeature_id_t feature, bool& hasFeature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.hasFeatureRPC((int32_t)feature);
    hasFeature = r.hasFeature;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setFeature(cameraFeature_id_t feature, double value)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setFeature1RPC((int32_t)feature, value);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getFeature(cameraFeature_id_t feature, double& value)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getFeature1RPC((int32_t)feature);
    value = r.value;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setFeature(cameraFeature_id_t feature, double value1, double value2)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setFeature2RPC((int32_t)feature, value1, value2);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getFeature(cameraFeature_id_t feature, double& value1, double& value2)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getFeature2RPC((int32_t)feature);
    value1 = r.value1;
    value2 = r.value2;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::hasOnOff(cameraFeature_id_t feature, bool& HasOnOff)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.hasOnOffRPC((int32_t)feature);
    HasOnOff = r.HasOnOff;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setActive(cameraFeature_id_t feature, bool onoff)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setActiveRPC((int32_t)feature, onoff);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getActive(cameraFeature_id_t feature, bool& isActive)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getActiveRPC((int32_t)feature);
    isActive = r.isActive;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::hasAuto(cameraFeature_id_t feature, bool& hasAuto)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.hasAutoRPC((int32_t)feature);
    hasAuto = r.hasAuto;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::hasManual(cameraFeature_id_t feature, bool& hasManual)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.hasManualRPC((int32_t)feature);
    hasManual = r.hasManual;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::hasOnePush(cameraFeature_id_t feature, bool& hasOnePush)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.hasOnePushRPC((int32_t)feature);
    hasOnePush = r.hasOnePush;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setMode(cameraFeature_id_t feature, yarp::dev::FeatureMode mode)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setModeRPC((int32_t)feature, mode);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getMode(cameraFeature_id_t feature, yarp::dev::FeatureMode& mode)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getModeRPC((int32_t)feature);
    mode = r.mode;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setOnePush(cameraFeature_id_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setOnePushRPC((int32_t)feature);
    return r;
}

/*
* IFrameGrabberControlsDC1394 specific interface methods
*/
ReturnValue FrameGrabber_nwc_yarp::getVideoModeMaskDC1394(unsigned int& val)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getVideoModeMaskDC1394RPC();
    val = r.val;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::getVideoModeDC1394(unsigned int& val)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getVideoModeDC1394RPC();
    val = r.val;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setVideoModeDC1394(int video_mode)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setVideoModeDC1394RPC(video_mode);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getFPSMaskDC1394(unsigned int& val)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getFPSMaskDC1394RPC();
    val = r.val;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::getFPSDC1394(unsigned int& fps)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getFPSDC1394RPC();
    fps = r.fps;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setFPSDC1394(int fps)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setFPSDC1394RPC(fps);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getISOSpeedDC1394(unsigned int& speed)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getISOSpeedDC1394RPC();
    speed = r.speed;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setISOSpeedDC1394(int speed)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setISOSpeedDC1394RPC(speed);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getColorCodingMaskDC1394(unsigned int video_mode, unsigned int& val)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getColorCodingMaskDC1394RPC(video_mode);
    val = r.val;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::getColorCodingDC1394(unsigned int& val)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getColorCodingDC1394RPC();
    val = r.val;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setColorCodingDC1394(int coding)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setColorCodingDC1394RPC(coding);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getFormat7MaxWindowDC1394(unsigned int& xdim,
                                   unsigned int& ydim,
                                   unsigned int& xstep,
                                   unsigned int& ystep,
                                   unsigned int& xoffstep,
                                   unsigned int& yoffstep)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getFormat7MaxWindowDC1394RPC();
    xdim = r.xdim;
    ydim = r.ydim;
    xstep = r.xstep;
    ystep = r.ystep;
    xoffstep = r.xoffstep;
    yoffstep = r.yoffstep;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::getFormat7WindowDC1394(unsigned int& xdim, unsigned int& ydim, int& x0, int& y0)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getFormat7WindowDC1394RPC();
    xdim = r.xdim;
    ydim = r.ydim;
    x0 = r.x0;
    y0 = r.y0;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setFormat7WindowDC1394(unsigned int xdim, unsigned int ydim, int x0, int y0)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setFormat7WindowDC1394RPC(xdim,ydim,x0,y0);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::setOperationModeDC1394(bool b1394b)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setOperationModeDC1394RPC(b1394b);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getOperationModeDC1394(bool& b1394b)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getOperationModeDC1394RPC();
    b1394b = r.b1394b;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setTransmissionDC1394(bool bTxON)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setTransmissionDC1394RPC(bTxON);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getTransmissionDC1394(bool& bTxON)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getTransmissionDC1394RPC();
    bTxON = r.bTxON;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setBroadcastDC1394(bool onoff)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setBroadcastDC1394RPC(onoff);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::setDefaultsDC1394()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setDefaultsDC1394RPC();
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::setResetDC1394()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setResetDC1394RPC();
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::setPowerDC1394(bool onoff)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setPowerDC1394RPC(onoff);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::setCaptureDC1394(bool bON)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setCaptureDC1394RPC(bON);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getBytesPerPacketDC1394(unsigned int& bpp)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getBytesPerPacketDC1394RPC();
    bpp = r.bpp;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setBytesPerPacketDC1394(unsigned int bpp)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setBytesPerPacketDC1394RPC(bpp);
    return r;
}

// END FrameGrabber_nwc_yarp

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

#include <yarp/proto/framegrabber/CameraVocabs.h>
#include <yarp/sig/ImageUtils.h>

namespace {
YARP_LOG_COMPONENT(FRAMEGRABBER_NWC_YARP, "yarp.devices.frameGrabber_nwc_yarp")
} // namespace

using namespace yarp::dev;

// BEGIN StreamReceiver
int StreamReceiver::lastHeight() const
{
    return m_lastHeight;
}

int StreamReceiver::lastWidth() const
{
    return m_lastWidth;
}

yarp::os::Stamp StreamReceiver::lastStamp() const
{
    return m_lastStamp;
}

template <typename ImageType>
bool StreamReceiver::lastImage(ImageType& image)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    bool ret {false};
    if (!port.isOpen()) {
        image.zero();
        ret = false;
    } else if (reader.check()) {
        ret = image.copy(*(reader.read(true)));
        reader.getEnvelope(m_lastStamp);
        m_lastHeight = image.height();
        m_lastWidth = image.width();
    } else {
        ret = image.copy(*(reader.lastRead()));
    }

    return ret;
}

bool StreamReceiver::open(const std::string& local,
               const std::string& remote,
               const std::string& carrier)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!port.open(local)) {
        yCError(FRAMEGRABBER_NWC_YARP) << "Failed to open " << local << "port.";
    }

    if (!remote.empty()) {
        yCInfo(FRAMEGRABBER_NWC_YARP) << "Connecting" << port.getName() << "to" << remote;
        if (!yarp::os::NetworkBase::connect(remote, port.getName(), carrier)) {
            yCError(FRAMEGRABBER_NWC_YARP) << "Failed to connect" << local << "to" << remote;
            return false;
        }
    } else {
        yCInfo(FRAMEGRABBER_NWC_YARP) << "No remote specified. Waiting for connection";
    }

    reader.attach(port);

    return true;
}

bool StreamReceiver::close()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_lastStamp = {0, 0.0};
    m_lastHeight = 0;
    m_lastWidth = 0;

    if (!port.isOpen()) {
        return true;
    }
    port.interrupt();
    port.close();

    return true;
}
// END StreamReceiver


// BEGIN FrameGrabberOf_ForwarderWithStream
template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
FrameGrabberOf_ForwarderWithStream<ImageType, IfVocab, ImgVocab>::FrameGrabberOf_ForwarderWithStream(yarp::os::Port& rpcPort) :
        yarp::proto::framegrabber::FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>(rpcPort)
{
}

template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
int FrameGrabberOf_ForwarderWithStream<ImageType, IfVocab, ImgVocab>::height() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_streamReceiver) {
        return yarp::proto::framegrabber::FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>::height();
    }

    return m_streamReceiver->lastHeight();
}

template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
int FrameGrabberOf_ForwarderWithStream<ImageType, IfVocab, ImgVocab>::width() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_streamReceiver) {
        return yarp::proto::framegrabber::FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>::width();
    }

    return m_streamReceiver->lastWidth();
}

template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
yarp::dev::ReturnValue FrameGrabberOf_ForwarderWithStream<ImageType, IfVocab, ImgVocab>::getImage(ImageType& image)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_streamReceiver) {
        return yarp::proto::framegrabber::FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>::getImage(image);
    }

    bool b= m_streamReceiver->lastImage(image);
    if (!b) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return ReturnValue_ok;
}

template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
yarp::dev::ReturnValue FrameGrabberOf_ForwarderWithStream<ImageType, IfVocab, ImgVocab>::getImageCrop(cropType_id_t cropType,
                                                                                    yarp::sig::VectorOf<std::pair<int, int>> vertices,
                                                                                    ImageType& image)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(!m_streamReceiver) {
        return yarp::proto::framegrabber::FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>::getImageCrop(cropType, vertices, image);
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

        if (!yarp::sig::utils::cropRect(full, vertices[0], vertices[1], image)) {
            yCError(FRAMEGRABBER_NWC_YARP, "GetImageCrop failed: utils::cropRect error: (%d, %d) (%d, %d)",
                vertices[0].first,
                vertices[0].second,
                vertices[1].first,
                vertices[1].second);
            return ReturnValue::return_code::return_value_error_method_failed;
        }
    }
    else if (cropType == YARP_CROP_LIST) {
        yCError(FRAMEGRABBER_NWC_YARP, "List type not yet implemented");
        return ReturnValue::return_code::return_value_error_not_implemented_by_device;
    }

    return ReturnValue_ok;
}


template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
void FrameGrabberOf_ForwarderWithStream<ImageType, IfVocab, ImgVocab>::setStreamReceiver(StreamReceiver* streamReceiver)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_streamReceiver = streamReceiver;
}
// END FrameGrabberOf_ForwarderWithStream



// BEGIN FrameGrabber_nwc_yarp
FrameGrabber_nwc_yarp::FrameGrabber_nwc_yarp() :
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelRgb>>(rpcPort),
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelMono>, VOCAB_FRAMEGRABBER_IMAGERAW>(rpcPort),
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelFloat>>(rpcPort),
        FrameGrabberOf_ForwarderWithStream<yarp::sig::FlexImage>(rpcPort)
{
}


bool FrameGrabber_nwc_yarp::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) { return false; }

    if (!m_no_stream) {
        if (!streamReceiver.open(m_local, m_remote, m_carrier)) {
            return false;
        }
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::setStreamReceiver(&streamReceiver);
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelMono>, VOCAB_FRAMEGRABBER_IMAGERAW>::setStreamReceiver(&streamReceiver);
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelFloat>>::setStreamReceiver(&streamReceiver);
        FrameGrabberOf_ForwarderWithStream<yarp::sig::FlexImage>::setStreamReceiver(&streamReceiver);
    }

    std::string rpc_local = m_local + "/rpc_client";
    std::string rpc_remote = m_remote + "/rpc";
    if (!rpcPort.open(rpc_local)) {
        yCError(FRAMEGRABBER_NWC_YARP) << "Failed to open " << rpc_local << "port.";
    }

    if (!m_remote.empty()) {
        yCInfo(FRAMEGRABBER_NWC_YARP) << "Connecting" << rpcPort.getName() << "to" << rpc_remote;
        if (!yarp::os::NetworkBase::connect(rpcPort.getName(), rpc_remote)) {
            yCError(FRAMEGRABBER_NWC_YARP) << "Failed to connect" << rpcPort.getName() << "to" << rpc_remote;
            return false;
        }
    } else {
        yCInfo(FRAMEGRABBER_NWC_YARP) << "No remote specified. Waiting for connection";
    }

    ///attach the message server
    if (!m_frameGrabber_RPC.yarp().attachAsClient(rpcPort))
    {
        yCError(FRAMEGRABBER_NWC_YARP, "Error! Cannot attach the port as a client");
        rpcPort.close();
        return false;
    }

    // Check the protocol version
    //if (!rpcPort.checkProtocolVersion()) {
    //    return false;
    //}

    return true;
}

bool FrameGrabber_nwc_yarp::close()
{
    rpcPort.interrupt();
    rpcPort.close();

    streamReceiver.close();

    return true;
}


yarp::os::Stamp FrameGrabber_nwc_yarp::getLastInputStamp()
{
    return streamReceiver.lastStamp();
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

ReturnValue FrameGrabber_nwc_yarp::hasFeature(int feature, bool& hasFeature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.hasFeatureRPC(feature);
    hasFeature = r.hasFeature;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setFeature(int feature, double value)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setFeature1RPC(feature,value);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getFeature(int feature, double& value)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getFeature1RPC(feature);
    value = r.value;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setFeature(int feature, double value1, double value2)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setFeature2RPC(feature,value1,value2);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getFeature(int feature, double& value1, double& value2)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getFeature2RPC(feature);
    value1 = r.value1;
    value2 = r.value2;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::hasOnOff(int feature, bool& HasOnOff)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.hasOnOffRPC(feature);
    HasOnOff = r.HasOnOff;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setActive(int feature, bool onoff)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setActiveRPC(feature,onoff);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getActive(int feature, bool& isActive)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getActiveRPC(feature);
    isActive = r.isActive;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::hasAuto(int feature, bool& hasAuto)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.hasAutoRPC(feature);
    hasAuto = r.hasAuto;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::hasManual(int feature, bool& hasManual)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.hasManualRPC(feature);
    hasManual = r.hasManual;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::hasOnePush(int feature, bool& hasOnePush)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.hasOnePushRPC(feature);
    hasOnePush = r.hasOnePush;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setMode(int feature, yarp::dev::FeatureMode mode)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setModeRPC(feature,mode);
    return r;
}

ReturnValue FrameGrabber_nwc_yarp::getMode(int feature, yarp::dev::FeatureMode& mode)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.getModeRPC(feature);
    mode = r.mode;
    return r.ret;
}

ReturnValue FrameGrabber_nwc_yarp::setOnePush(int feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_frameGrabber_RPC.setOnePushRPC(feature);
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

/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_FRAMEGRABBERMSGSRIMPL_H
#define YARP_DEV_FRAMEGRABBERMSGSRIMPL_H

#include "FrameGrabberMsgs.h"
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IFrameGrabberControlsDC1394.h>
#include <yarp/dev/IRgbVisualParams.h>


class FrameGrabberMsgsImpl : public FrameGrabberMsgs
{
    private:
    yarp::dev::IRgbVisualParams*            m_irgbvp = nullptr;
    yarp::dev::IFrameGrabberControls*       m_ictrls = nullptr;
    yarp::dev::IFrameGrabberControlsDC1394* m_ictrls1394 = nullptr;
    yarp::dev::IFrameGrabberImage*          m_iframegrabimg = nullptr;
    yarp::dev::IFrameGrabberImageRaw*       m_iframegrabimgraw = nullptr;
    std::mutex                              m_mutex;

    public:
    FrameGrabberMsgsImpl(yarp::dev::IRgbVisualParams* _irgbvp,
                         yarp::dev::IFrameGrabberControls* _ictrls,
                         yarp::dev::IFrameGrabberControlsDC1394* _ictrls1394,
                         yarp::dev::IFrameGrabberImage* _iframegrabimg,
                         yarp::dev::IFrameGrabberImageRaw* _iframegrabimgraw);

    virtual IRGBVisualParamsMsgs_return_getRgbWidth getRgbWidthRPC() override;
    virtual IRGBVisualParamsMsgs_return_getRgbHeight getRgbHeightRPC() override;
    virtual IRGBVisualParamsMsgs_return_getRgbSupportedCfg getRgbSupportedConfigurationsRPC() override;
    virtual IRGBVisualParamsMsgs_return_getRgbResolution getRgbResolutionRPC() override;
    virtual yarp::dev::ReturnValue setRgbResolutionRPC(const std::int32_t width, const std::int32_t height) override;
    virtual IRGBVisualParamsMsgs_return_getRgbFOV getRgbFOVRPC() override;
    virtual yarp::dev::ReturnValue setRgbFOVRPC(const double horizontalFov, const double verticalFov) override;
    virtual IRGBVisualParamsMsgs_return_getRgbIntrinsicParam getRgbIntrinsicParamRPC() override;
    virtual IRGBVisualParamsMsgs_return_getRgbMirroring getRgbMirroringRPC() override;
    virtual yarp::dev::ReturnValue setRgbMirroringRPC(const bool mirror) override;
    virtual IFrameGrabberControlMsgs_return_getCameraDescription getCameraDescriptionRPC() override;
    virtual IFrameGrabberControlMsgs_return_hasFeature hasFeatureRPC(const std::int32_t feature) override;
    virtual yarp::dev::ReturnValue setFeature1RPC(const std::int32_t feature, const double value) override;
    virtual IFrameGrabberControlMsgs_return_getFeature1 getFeature1RPC(const std::int32_t feature) override;
    virtual yarp::dev::ReturnValue setFeature2RPC(const std::int32_t feature, const double value1, const double value2) override;
    virtual IFrameGrabberControlMsgs_return_getFeature2 getFeature2RPC(const std::int32_t feature) override;
    virtual IFrameGrabberControlMsgs_return_hasOnOff hasOnOffRPC(const std::int32_t feature) override;
    virtual yarp::dev::ReturnValue setActiveRPC(const std::int32_t feature, const bool onoff) override;
    virtual IFrameGrabberControlMsgs_return_getActive getActiveRPC(const std::int32_t feature) override;
    virtual IFrameGrabberControlMsgs_return_hasAuto hasAutoRPC(const std::int32_t feature) override;
    virtual IFrameGrabberControlMsgs_return_hasManual hasManualRPC(const std::int32_t feature) override;
    virtual IFrameGrabberControlMsgs_return_hasOnePush hasOnePushRPC(const std::int32_t feature) override;
    virtual yarp::dev::ReturnValue setModeRPC(const std::int32_t feature, const yarp::dev::FeatureMode mode) override;
    virtual IFrameGrabberControlMsgs_return_getMode getModeRPC(const std::int32_t feature) override;
    virtual yarp::dev::ReturnValue setOnePushRPC(const std::int32_t feature) override;
    virtual IFrameGrabberControlDC1394Msgs_return_getVideoModeMaskDC1394 getVideoModeMaskDC1394RPC() override;
    virtual IFrameGrabberControlDC1394Msgs_return_getVideoModeDC1394 getVideoModeDC1394RPC() override;
    virtual yarp::dev::ReturnValue setVideoModeDC1394RPC(const std::int32_t videomode) override;
    virtual IFrameGrabberControlDC1394Msgs_return_getFPSMaskDC1394 getFPSMaskDC1394RPC() override;
    virtual IFrameGrabberControlDC1394Msgs_return_getFPSDC1394 getFPSDC1394RPC() override;
    virtual yarp::dev::ReturnValue setFPSDC1394RPC(const std::int32_t fps) override;
    virtual IFrameGrabberControlDC1394Msgs_return_getISOSpeedDC1394 getISOSpeedDC1394RPC() override;
    virtual yarp::dev::ReturnValue setISOSpeedDC1394RPC(const std::int32_t speed) override;
    virtual IFrameGrabberControlDC1394Msgs_return_getColorCodingMaskDC1394 getColorCodingMaskDC1394RPC(const std::int32_t videomode) override;
    virtual IFrameGrabberControlDC1394Msgs_return_getColorCodingDC1394 getColorCodingDC1394RPC() override;
    virtual yarp::dev::ReturnValue setColorCodingDC1394RPC(const std::int32_t coding) override;
    virtual IFrameGrabberControlDC1394Msgs_return_getFormat7MaxWindowDC1394 getFormat7MaxWindowDC1394RPC() override;
    virtual IFrameGrabberControlDC1394Msgs_return_getFormat7WindowDC1394 getFormat7WindowDC1394RPC() override;
    virtual yarp::dev::ReturnValue setFormat7WindowDC1394RPC(const std::int32_t xdim, const std::int32_t ydim, const std::int32_t x0, const std::int32_t y0) override;
    virtual yarp::dev::ReturnValue setOperationModeDC1394RPC(const bool b1394b) override;
    virtual IFrameGrabberControlDC1394Msgs_return_getOperationModeDC1394 getOperationModeDC1394RPC() override;
    virtual yarp::dev::ReturnValue setTransmissionDC1394RPC(const bool bTxON) override;
    virtual IFrameGrabberControlDC1394Msgs_return_getTransmissionDC1394 getTransmissionDC1394RPC() override;
    virtual yarp::dev::ReturnValue setBroadcastDC1394RPC(const bool onoff) override;
    virtual yarp::dev::ReturnValue setDefaultsDC1394RPC() override;
    virtual yarp::dev::ReturnValue setResetDC1394RPC() override;
    virtual yarp::dev::ReturnValue setPowerDC1394RPC(const bool onoff) override;
    virtual yarp::dev::ReturnValue setCaptureDC1394RPC(const bool bON) override;
    virtual IFrameGrabberControlDC1394Msgs_return_getBytesPerPacketDC1394 getBytesPerPacketDC1394RPC() override;
    virtual yarp::dev::ReturnValue setBytesPerPacketDC1394RPC(const std::int32_t bpp) override;

    virtual IFrameGrabberImageOf_return_getHeight getHeightRPC() override;
    virtual IFrameGrabberImageOf_return_getWidth getWidthRPC() override;
    virtual IFrameGrabberImageOf_return_getImage getImageRPC() override;
    virtual IFrameGrabberImageOf_return_getImageCrop getImageCropRPC(const yarp::dev::cropType_id_t type, const std::vector<yarp::dev::vertex_t>& vertices) override;

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_FRAMEGRABBERMSGSRIMPL_H

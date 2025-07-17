/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_RGBDSENSORMSGSIMPL_H
#define YARP_DEV_RGBDSENSORMSGSIMPL_H

#include "RGBDSensorMsgs.h"

#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IRgbVisualParams.h>
#include <yarp/dev/IDepthVisualParams.h>


class RGBDSensorMsgsImpl : public RGBDSensorMsgs
{
    private:
    yarp::dev::IRGBDSensor*                 m_irgbd = nullptr;
    yarp::dev::IFrameGrabberControls*       m_ictrls = nullptr;
    std::mutex                              m_mutex;

    public:
    RGBDSensorMsgsImpl(yarp::dev::IRGBDSensor* _irgbd, yarp::dev::IFrameGrabberControls* _ictrls);

    //IRGBDSensor
    virtual IRGBDMsgs_return_getExtrinsic getExtrinsicParamRPC() override;
    virtual IRGBDMsgs_return_getLastErrorMsg getLastErrorMsgRPC() override;
    virtual IRGBDMsgs_return_getRgbImage getRgbImageRPC() override;
    virtual IRGBDMsgs_return_getDepthImage getDepthImageRPC() override;
    virtual IRGBDMsgs_return_getImages getImagesRPC() override;
    virtual IRGBDMsgs_return_getSensorStatus getSensorStatusRPC() override;

    //IRgbVisualParams
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

    //IDepthVisualParams
    virtual IDepthVisualParamsMsgs_return_getDepthWidth getDepthWidthRPC() override;
    virtual IDepthVisualParamsMsgs_return_getDepthHeight getDepthHeightRPC() override;
    virtual IDepthVisualParamsMsgs_return_getDepthResolution getDepthResolutionRPC() override;
    virtual yarp::dev::ReturnValue setDepthResolutionRPC(const std::int32_t width, const std::int32_t height) override;
    virtual IDepthVisualParamsMsgs_return_getDepthFOV getDepthFOVRPC() override;
    virtual yarp::dev::ReturnValue setDepthFOVRPC(const double horizontalFov, const double verticalFov) override;
    virtual IDepthVisualParamsMsgs_return_getDepthAccuracy getDepthAccuracyRPC() override;
    virtual yarp::dev::ReturnValue setDepthAccuracyRPC(const double accuracy) override;
    virtual IDepthVisualParamsMsgs_return_getDepthClipPlanes getDepthClipPlanesRPC() override;
    virtual yarp::dev::ReturnValue setDepthClipPlanesRPC(const double nearPlane, const double farPlane) override;
    virtual IDepthVisualParamsMsgs_return_getDepthMirroring getDepthMirroringRPC() override;
    virtual yarp::dev::ReturnValue setDepthMirroringRPC(const bool mirror) override;
    virtual IDepthVisualParamsMsgs_return_getDepthIntrinsicParam getDepthIntrinsicParamRPC() override;

    //IFrameGrabberControls
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

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_RGBDSENSORMSGSIMPL_H

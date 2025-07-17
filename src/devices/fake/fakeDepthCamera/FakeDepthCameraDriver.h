/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FAKEDEPTHCAMERADRIVER_H
#define YARP_FAKEDEPTHCAMERADRIVER_H

#include "FakeDepthCameraDriver_mini.h"
#include <yarp/dev/IFrameGrabberControls.h>

/**
 * @ingroup dev_impl_fake
 *
 * \brief `fakeDepthCamera`: Documentation to be added
 *
 * Parameters required by this device are shown in class: FakeDepthCameraDriver_ParamsParser
 *
*/

class FakeDepthCameraDriver :
        public FakeDepthCameraDriver_mini,
        public yarp::dev::IFrameGrabberControls
{
public:
    FakeDepthCameraDriver();
    ~FakeDepthCameraDriver() override;

    // DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // IFrameGrabberControls
    yarp::dev::ReturnValue getCameraDescription(yarp::dev::CameraDescriptor& camera) override;
    yarp::dev::ReturnValue hasFeature(int feature, bool& hasFeature) override;
    yarp::dev::ReturnValue setFeature(int feature, double value) override;
    yarp::dev::ReturnValue getFeature(int feature, double& value) override;
    yarp::dev::ReturnValue setFeature(int feature, double value1, double value2) override;
    yarp::dev::ReturnValue getFeature(int feature, double& value1, double& value2) override;
    yarp::dev::ReturnValue hasOnOff(int feature, bool& HasOnOff) override;
    yarp::dev::ReturnValue setActive(int feature, bool onoff) override;
    yarp::dev::ReturnValue getActive(int feature, bool& isActive) override;
    yarp::dev::ReturnValue hasAuto(int feature, bool& hasAuto) override;
    yarp::dev::ReturnValue hasManual(int feature, bool& hasManual) override;
    yarp::dev::ReturnValue hasOnePush(int feature, bool& hasOnePush) override;
    yarp::dev::ReturnValue setMode(int feature, yarp::dev::FeatureMode mode) override;
    yarp::dev::ReturnValue getMode(int feature, yarp::dev::FeatureMode& mode) override;
    yarp::dev::ReturnValue setOnePush(int feature) override;
};

#endif // YARP_FAKEDEPTHCAMERADRIVER_H

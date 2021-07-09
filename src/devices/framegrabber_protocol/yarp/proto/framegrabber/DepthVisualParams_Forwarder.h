/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FRAMEGRABBER_PROTOCOL_DEPTHVISUALPARAMS_FORWARDER_H
#define YARP_FRAMEGRABBER_PROTOCOL_DEPTHVISUALPARAMS_FORWARDER_H

#include <yarp/dev/IDepthVisualParams.h>
#include <yarp/os/Port.h>

namespace yarp {
namespace proto {
namespace framegrabber {

class DepthVisualParams_Forwarder :
        public yarp::dev::IDepthVisualParams
{
private:
    yarp::os::Port& m_port;

public:
    DepthVisualParams_Forwarder(yarp::os::Port& port);
    ~DepthVisualParams_Forwarder() override = default;

    int getDepthHeight() override;
    int getDepthWidth() override;
    bool setDepthResolution(int width, int height) override;
    bool getDepthFOV(double& horizontalFov, double& verticalFov) override;
    bool setDepthFOV(double horizontalFov, double verticalFov) override;
    double getDepthAccuracy() override;
    bool setDepthAccuracy(double accuracy) override;
    bool getDepthClipPlanes(double& nearPlane, double& farPlane) override;
    bool setDepthClipPlanes(double nearPlane, double farPlane) override;
    bool getDepthIntrinsicParam(yarp::os::Property& intrinsic) override;
    bool getDepthMirroring(bool& mirror) override;
    bool setDepthMirroring(bool mirror) override;
};

} // namespace framegrabber
} // namespace proto
} // namespace yarp

#endif // YARP_FRAMEGRABBER_PROTOCOL_DEPTHVISUALPARAMS_FORWARDER_H

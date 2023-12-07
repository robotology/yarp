/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FRAMEGRABBER_PROTOCOL_RGBVISUALPARAMS_RESPONDER_H
#define YARP_FRAMEGRABBER_PROTOCOL_RGBVISUALPARAMS_RESPONDER_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IRgbVisualParams.h>

namespace yarp::proto::framegrabber {

class RgbVisualParams_Responder :
        public yarp::dev::DeviceResponder
{
private:
    yarp::dev::IRgbVisualParams* iRgbVisual {nullptr};

public:
    RgbVisualParams_Responder() = default;
    ~RgbVisualParams_Responder() override = default;

    bool configure(yarp::dev::IRgbVisualParams* interface);
    bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) override;
};

} // namespace yarp::proto::framegrabber

#endif // YARP_FRAMEGRABBER_PROTOCOL_RGBVISUALPARAMS_RESPONDER_H

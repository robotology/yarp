/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_CARRIER_DEPTHIMAGECONVERTER_H
#define YARP_CARRIER_DEPTHIMAGECONVERTER_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Things.h>
#include <yarp/os/MonitorObject.h>
#include <yarp/sig/Image.h>


class DepthImageConverter : public yarp::os::MonitorObject
{
public:
    bool create(const yarp::os::Property& options) override;
    void destroy() override;

    bool setparam(const yarp::os::Property& params) override;
    bool getparam(yarp::os::Property& params) override;

    bool accept(yarp::os::Things& thing) override;
    yarp::os::Things& update(yarp::os::Things& thing) override;

private:

    double min, max;
    yarp::os::Bottle bt;
    yarp::os::Things th;
    float **inMatrix;
    unsigned char **outMatrix;
    yarp::sig::FlexImage outImg;
};

#endif  // YARP_CARRIER_DEPTHIMAGECONVERTER_H

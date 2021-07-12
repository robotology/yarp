/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_CARRIER_SEGMENTATION_CONVERTER_H
#define YARP_CARRIER_SEGMENTATION_CONVERTER_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Things.h>
#include <yarp/os/MonitorObject.h>
#include <yarp/sig/Image.h>

//example usage:
//yarp connect /segmentationimage:o /yarpview/img:i tcp+recv.portmonitor+type.dll+file.segmentationimage_to_rgb

struct rgbColor
{
    char r;
    char g;
    char b;
};

class SegmentationImageConverter : public yarp::os::MonitorObject
{
public:
    bool create(const yarp::os::Property& options) override;
    void destroy() override;

    bool setparam(const yarp::os::Property& params) override;
    bool getparam(yarp::os::Property& params) override;

    bool accept(yarp::os::Things& thing) override;
    yarp::os::Things& update(yarp::os::Things& thing) override;

private:
    int max_colors;
    yarp::os::Bottle bt;
    yarp::os::Things th;
    yarp::sig::FlexImage outImg;
    std::map<int, rgbColor> colormap;
};

#endif  // YARP_CARRIER_SEGMENTATION_CONVERTER_H

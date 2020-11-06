/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ZLIB_CARRIER_ZFPPORTMONITOR_H
#define YARP_ZLIB_CARRIER_ZFPPORTMONITOR_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Things.h>
#include <yarp/sig/Image.h>
#include <yarp/os/MonitorObject.h>


class ZlibMonitorObject : public yarp::os::MonitorObject
{
public:
    bool create(const yarp::os::Property& options) override;
    void destroy() override;

    bool setparam(const yarp::os::Property& params) override;
    bool getparam(yarp::os::Property& params) override;

    bool accept(yarp::os::Things& thing) override;
    yarp::os::Things& update(yarp::os::Things& thing) override;
protected:
    int compress(char* in, size_t in_size, char* out, size_t out_size);
    int decompress(char* in, size_t in_size, char* out, size_t out_size);
private:
    yarp::os::Things th;
    yarp::os::Bottle data;
    bool shouldCompress;
    void  *buffer;
    float *compressed;
    float *decompressed;
};

#endif

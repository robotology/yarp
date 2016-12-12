/*
 * Copyright (C) 2016 iCub Facility
 * Authors: Nicolo' Genesio
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_ZFP_CARRIER_ZFPPORTMONITOR_H
#define YARP_ZFP_CARRIER_ZFPPORTMONITOR_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Things.h>
#include <yarp/sig/Image.h>
#include <yarp/os/MonitorObject.h>


class ZfpMonitorObject : public yarp::os::MonitorObject
{
public:
    bool create(const yarp::os::Property& options);
    void destroy(void);

    bool setparam(const yarp::os::Property& params);
    bool getparam(yarp::os::Property& params);

    bool accept(yarp::os::Things& thing);
    yarp::os::Things& update(yarp::os::Things& thing);
protected:
    int compress(float* array, float* &compressed, int &zfpsize, int nx, int ny, float tolerance);
    int decompress(float* array, float* &decompressed, int zfpsize, int nx, int ny, float tolerance);

private:
    yarp::os::Things th;
    yarp::os::Bottle data;
    yarp::sig::ImageOf<yarp::sig::PixelFloat> imageOut;
    bool shouldCompress;
    float *compressed;
    float *decompressed;
};

#endif

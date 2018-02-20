/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
 * Authors: Nicolò Genesio
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
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
    bool create(const yarp::os::Property& options) override;
    void destroy(void) override;

    bool setparam(const yarp::os::Property& params) override;
    bool getparam(yarp::os::Property& params) override;

    bool accept(yarp::os::Things& thing) override;
    yarp::os::Things& update(yarp::os::Things& thing) override;
protected:
    int compress(float* array, float* &compressed, int &zfpsize, int nx, int ny, float tolerance);
    int decompress(float* array, float* &decompressed, int zfpsize, int nx, int ny, float tolerance);
    void resizeF(float* &array, int newSize);
    void resizeV(void* &array, int newSize);
private:
    yarp::os::Things th;
    yarp::os::Bottle data;
    yarp::sig::ImageOf<yarp::sig::PixelFloat> imageOut;
    bool shouldCompress;
    void  *buffer;
    float *compressed;
    float *decompressed;
    int sizeToAllocate;
    int sizeToAllocateB;
};

#endif

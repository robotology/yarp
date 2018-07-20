/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

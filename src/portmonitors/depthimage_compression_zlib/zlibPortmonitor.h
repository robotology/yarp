/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    int compressData  (const unsigned char* in, const size_t& in_size, unsigned char* out, size_t& out_size);
    int decompressData(const unsigned char* in, const size_t& in_size, unsigned char* out, size_t& out_size);

private:
    yarp::os::Things m_th;
    yarp::os::Bottle m_data;
    bool             m_shouldCompress;
    yarp::sig::ImageOf<yarp::sig::PixelFloat> m_imageOut;
};

#endif

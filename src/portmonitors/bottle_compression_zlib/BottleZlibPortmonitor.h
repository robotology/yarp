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

 /**
  * \brief `bottle_compression_zlib_portmonitor`: Portmonitor plugin for compression and decompression of bottles (or yarp data types castable to bottle) using zlib library.
  *
  * Example usage:
  * yarp connect /src /dest tcp+send.portmonitor+file.bottle_compression_zlib+recv.portmonitor+file.bottle_compression_zlib+type.dll
  */
class BottleZlibMonitorObject : public yarp::os::MonitorObject
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
};

#endif

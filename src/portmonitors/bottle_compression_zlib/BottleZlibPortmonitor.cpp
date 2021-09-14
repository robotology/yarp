/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "BottleZlibPortmonitor.h"

#include <yarp/os/LogStream.h>
#include <yarp/os/LogComponent.h>
#include <yarp/sig/Image.h>

#include <cstring>
#include <cmath>
#include <algorithm>

#include <zlib.h>

using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(BOTTLE_ZLIB_MONITOR,
                   "yarp.carrier.portmonitor.bottle_zlib",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
}


bool BottleZlibMonitorObject::create(const yarp::os::Property& options)
{
    m_shouldCompress = (options.find("sender_side").asBool());
    return true;
}

void BottleZlibMonitorObject::destroy()
{
}

bool BottleZlibMonitorObject::setparam(const yarp::os::Property& params)
{
    return false;
}

bool BottleZlibMonitorObject::getparam(yarp::os::Property& params)
{
    return false;
}

bool BottleZlibMonitorObject::accept(yarp::os::Things& thing)
{
  /*
    //FIXME by DrDanz using c++17
    if(m_shouldCompress)
    {
        //sender side / compressor
        auto* b = thing.cast_as<yarp::os::Portable>();
        if(b == nullptr)
        {
            yCError(BOTTLE_ZLIB_MONITOR, "Expected type Bottle in sender side, but got wrong data type!");
            return false;
        }
    }
    else
    {
        //receiver side / decompressor
        auto* b = thing.cast_as<yarp::os::Portable>();
        if(b == nullptr)
        {
            yCError(BOTTLE_ZLIB_MONITOR, "Expected type Bottle in receiver side, but got wrong data type!");
            return false;
        }
    }
    */
    return true;
}

yarp::os::Things& BottleZlibMonitorObject::update(yarp::os::Things& thing)
{
   if(m_shouldCompress)
   {
        //sender side / compressor
        //auto zzz = thing.cast_as<yarp::os::Portable>(); //FIXME by DrDanz using c++17
        yarp::os::Portable* pwrite = dynamic_cast<yarp::os::Portable*>(thing.getPortWriter());//.cast_as<yarp::os::Portable>();
        yarp::os::Bottle b;
        yarp::os::Portable::copyPortable(*pwrite,b);

        size_t sizeUncompressed = 0;
        const unsigned char* uncompressedData = (const unsigned char*)(b.toBinary(&sizeUncompressed));

        size_t sizeCompressed = (sizeUncompressed * 1.1) + 12;
        unsigned char* compressedData = (unsigned char*) malloc (sizeCompressed);

        bool ret= compressData(uncompressedData, sizeUncompressed, compressedData, sizeCompressed);
        if(!ret)
        {
            yCError(BOTTLE_ZLIB_MONITOR, "Failed to compress, exiting...");
            free(compressedData);
            return thing;
        }

        m_data.clear();
        Value v(compressedData, sizeCompressed);
        m_data.addInt32(sizeUncompressed);
        m_data.add(v);
        m_th.setPortWriter(&m_data);

        free(compressedData);
   }
   else
   {
       //receiver side / decompressor
       yarp::os::Bottle* b = thing.cast_as<yarp::os::Bottle>();

       size_t sizeUncompressed = b->get(0).asInt32();
       size_t sizeCompressed=b->get(1).asBlobLength();
       const unsigned char* CompressedData = (const unsigned char*) b->get(1).asBlob();

       unsigned char* uncompressedData = (unsigned char*)malloc(sizeUncompressed);

       bool ret = decompressData(CompressedData, sizeCompressed, uncompressedData, sizeUncompressed);
       if(!ret)
       {
           yCError(BOTTLE_ZLIB_MONITOR, "Failed to decompress, exiting...");
           free(uncompressedData);
           return thing;
       }

       m_data.fromBinary((const char*)(uncompressedData),sizeUncompressed);
       m_th.setPortWriter(&m_data);

       free(uncompressedData);
   }

    return m_th;
}

int BottleZlibMonitorObject::compressData(const unsigned char* in, const size_t& in_size, unsigned char* out, size_t& out_size)
{
    int z_result = compress((Bytef*)out, (uLongf*)&out_size, (Bytef*)in, in_size);
    switch (z_result)
    {
    case Z_OK:
        break;

    case Z_MEM_ERROR:
        yCError(BOTTLE_ZLIB_MONITOR, "zlib compression: out of memory");
        return false;
        break;

    case Z_BUF_ERROR:
        yCError(BOTTLE_ZLIB_MONITOR, "zlib compression: output buffer wasn't large enough");
        return false;
        break;
    }

    return true;
}

int BottleZlibMonitorObject::decompressData(const unsigned char* in, const size_t& in_size, unsigned char* out, size_t& out_size)
{
    int z_result = uncompress((Bytef*)out, (uLongf*)&out_size, (const Bytef*)in, in_size);
    switch (z_result)
    {
    case Z_OK:
        break;

    case Z_MEM_ERROR:
        yCError(BOTTLE_ZLIB_MONITOR, "zlib compression: out of memory");
        return false;
        break;

    case Z_BUF_ERROR:
        yCError(BOTTLE_ZLIB_MONITOR, "zlib compression: output buffer wasn't large enough");
        return false;
        break;

    case Z_DATA_ERROR:
        yCError(BOTTLE_ZLIB_MONITOR, "zlib compression: file contains corrupted data");
        return false;
        break;
    }

    return true;
}

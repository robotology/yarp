/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "zlibPortmonitor.h"

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
YARP_LOG_COMPONENT(ZLIBMONITOR,
                   "yarp.carrier.portmonitor.zlib",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
}


bool ZlibMonitorObject::create(const yarp::os::Property& options)
{
    m_shouldCompress = (options.find("sender_side").asBool());
    return true;
}

void ZlibMonitorObject::destroy()
{
}

bool ZlibMonitorObject::setparam(const yarp::os::Property& params)
{
    return false;
}

bool ZlibMonitorObject::getparam(yarp::os::Property& params)
{
    return false;
}

bool ZlibMonitorObject::accept(yarp::os::Things& thing)
{
    if(m_shouldCompress)
    {
        //sender side / compressor
        auto* b = thing.cast_as<ImageOf<PixelFloat>>();
        if(b == nullptr)
        {
            yCError(ZLIBMONITOR, "Expected type ImageOf<PixelFloat> in sender side, but got wrong data type!");
            return false;
        }
    }
    else
    {
        //receiver side / decompressor
        auto* b = thing.cast_as<Bottle>();
        if(b == nullptr)
        {
            yCError(ZLIBMONITOR, "Expected type Bottle in receiver side, but got wrong data type!");
            return false;
        }
    }
    return true;
}

yarp::os::Things& ZlibMonitorObject::update(yarp::os::Things& thing)
{
   if(m_shouldCompress)
   {
        //sender side / compressor
       //it receives an image, it sends a bottle to the network
        auto* b = thing.cast_as<ImageOf<PixelFloat>>();

        size_t sizeUncompressed= b->getRawImageSize();
        const unsigned char* uncompressedData = b->getRawImage();

        size_t sizeCompressed = (sizeUncompressed * 1.1) + 12;
        unsigned char* compressedData = (unsigned char*) malloc (sizeCompressed);

        bool ret= compressData(uncompressedData, sizeUncompressed, compressedData, sizeCompressed);
        if(!ret)
        {
            yCError(ZLIBMONITOR, "Failed to compress, exiting...");
            free(compressedData);
            return thing;
        }

        m_data.clear();
        Value v(compressedData, sizeCompressed);
        m_data.addInt32(b->width());
        m_data.addInt32(b->height());
        m_data.addInt32(sizeCompressed);
        m_data.addInt32(sizeUncompressed);
        m_data.add(v);
        m_th.setPortWriter(&m_data);

        free(compressedData);
   }
   else
   {
       //receiver side / decompressor
       //it receives a bottle from the network, it creates an image
       Bottle* b= thing.cast_as<Bottle>();

       size_t w = b->get(0).asInt32();
       size_t h = b->get(1).asInt32();
       size_t check_sizeCompressed = b->get(2).asInt32();
       size_t check_sizeUncompressed = b->get(3).asInt32();
       size_t sizeCompressed=b->get(4).asBlobLength();
       const unsigned char* CompressedData = (const unsigned char*) b->get(4).asBlob();

       size_t sizeUncompressed = w * h * 4;
       if (check_sizeUncompressed != sizeUncompressed ||
           check_sizeCompressed != sizeCompressed)
       {
           yCError(ZLIBMONITOR, "Invalid data received: wrong blob size?");
           return thing;
       }
       unsigned char* uncompressedData = (unsigned char*)malloc(sizeUncompressed);

       bool ret = decompressData(CompressedData, sizeCompressed, uncompressedData, sizeUncompressed);
       if(!ret)
       {
           yCError(ZLIBMONITOR, "Failed to decompress, exiting...");
           free(uncompressedData);
           return thing;
       }

       m_imageOut.resize(w, h);
       memcpy(m_imageOut.getRawImage(), uncompressedData, sizeUncompressed);
       m_th.setPortWriter(&m_imageOut);

       free(uncompressedData);
   }

    return m_th;
}

int ZlibMonitorObject::compressData(const unsigned char* in, const size_t& in_size, unsigned char* out, size_t& out_size)
{
    int z_result = compress((Bytef*)out, (uLongf*)&out_size, (Bytef*)in, in_size);
    switch (z_result)
    {
    case Z_OK:
        break;

    case Z_MEM_ERROR:
        yCError(ZLIBMONITOR, "zlib compression: out of memory");
        return false;
        break;

    case Z_BUF_ERROR:
        yCError(ZLIBMONITOR, "zlib compression: output buffer wasn't large enough");
        return false;
        break;
    }

    return true;
}

int ZlibMonitorObject::decompressData(const unsigned char* in, const size_t& in_size, unsigned char* out, size_t& out_size)
{
    int z_result = uncompress((Bytef*)out, (uLongf*)&out_size, (const Bytef*)in, in_size);
    switch (z_result)
    {
    case Z_OK:
        break;

    case Z_MEM_ERROR:
        yCError(ZLIBMONITOR, "zlib compression: out of memory");
        return false;
        break;

    case Z_BUF_ERROR:
        yCError(ZLIBMONITOR, "zlib compression: output buffer wasn't large enough");
        return false;
        break;

    case Z_DATA_ERROR:
        yCError(ZLIBMONITOR, "zlib compression: file contains corrupted data");
        return false;
        break;
    }

    return true;
}

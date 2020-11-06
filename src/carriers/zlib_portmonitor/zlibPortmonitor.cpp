/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "zlibPortmonitor.h"

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
    shouldCompress = (options.find("sender_side").asBool());
    compressed = nullptr;
    decompressed = nullptr;
    buffer = nullptr;
    return true;
}

void ZlibMonitorObject::destroy()
{
    if(compressed){
        free(compressed);
        compressed = nullptr;
    }

    if(buffer){
        free(buffer);
        buffer = nullptr;
    }

    if(decompressed){
        free(decompressed);
        decompressed = nullptr;
    }
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
    if(shouldCompress)
    {
        yarp::os::Bottle* b = thing.cast_as<yarp::os::Bottle>();
        if(b == nullptr)
        {
            yCError(ZLIBMONITOR, "Expected type ImageOf<PixelFloat> in sender side, but got wrong data type!");
            return false;
        }
    }
    else
    {
        yarp::os::Bottle* b = thing.cast_as<yarp::os::Bottle>();
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
   if(shouldCompress)
   {
       yarp::os::Bottle* b = thing.cast_as<yarp::os::Bottle>();
        // .... buffer, len
        size_t sizeCompressed;
        const char* bin = b->toBinary(&sizeCompressed);
        compress(bin, sizeCompressed, sizeCompressed,  , );
        if(!compressed)
        {
            yCError(ZLIBMONITOR, "Failed to compress, exiting...");
            return thing;
        }
        data.clear();
        data.addInt32(sizeCompressed);
        Value v(compressed, sizeCompressed);
        data.add(v);
        th.setPortWriter(&data);
   }
   else
   {

       Bottle* compressedbt= thing.cast_as<Bottle>();

       int width=compressedbt->get(0).asInt32();
       int height=compressedbt->get(1).asInt32();
       int sizeCompressed=compressedbt->get(2).asInt32();
       // cast thing to compressed.
       decompress((float*)compressedbt->get(3).asBlob(), decompressed, sizeCompressed, width, height,1e-3);

       if(!decompressed)
       {
           yCError(ZLIBMONITOR, "Failed to decompress, exiting...");
           return thing;
       }
       memcpy(imageOut.getRawImage(),decompressed,width*height*4);
       th.setPortWriter(&imageOut);

   }

    return th;
}

int ZlibMonitorObject::compress(char* in, size_t in_size, char* out, size_t out_size)
{
    //read the compressed data
    char* dataReadInCompressed = new char[sizeDataCompressed];
    br = fread(dataReadInCompressed, 1, sizeDataCompressed, fp);
    fclose(fp);

    if (br != sizeDataCompressed) { yError() << "problems reading file!"; delete[] dataReadInCompressed; return false; }

    size_t h = ((size_t*)(dataReadInCompressed))[0]; //byte 0
    size_t w = ((size_t*)(dataReadInCompressed))[1]; //byte 8, because size_t is 8 bytes long
    size_t hds = 2 * sizeof(size_t); //16 bytes

    dest.resize(w, h);
    unsigned char* destbuff = dest.getRawImage();
    //this is the size of the image
    size_t sizeDataUncompressed = dest.getRawImageSize();
    //this is the size of the buffer. Extra space is required for temporary operations (I choose arbitrarily *2)
    size_t sizeDataUncompressedExtra = sizeDataUncompressed * 2;

    char* dataUncompressed = new char[sizeDataUncompressedExtra];

    int z_result = uncompress((Bytef*)dataUncompressed, (uLongf*)&sizeDataUncompressedExtra, (const Bytef*)dataReadInCompressed + hds, sizeDataCompressed - hds);
    switch (z_result)
    {
    case Z_OK:
        break;

    case Z_MEM_ERROR:
        yCError(ZLIBMONITOR, "zlib compression: out of memory");
        delete[] dataUncompressed;
        return false;
        break;

    case Z_BUF_ERROR:
        yCError(ZLIBMONITOR, "zlib compression: output buffer wasn't large enough");
        delete[] dataUncompressed;
        return false;
        break;

    case Z_DATA_ERROR:
        yCError(ZLIBMONITOR, "zlib compression: file contains corrupted data");
        delete[] dataUncompressed;
        return false;
        break;
    }

    //here I am copy only the size of the image, obviously the extra space is not needed anymore.
    for (size_t i = 0; i < sizeDataUncompressed; i++)
    {
        destbuff[i] = dataUncompressed[i];
    }

    delete[] dataUncompressed;
    return true;
}

int ZlibMonitorObject::decompress(char* in, size_t in_size, char* out, size_t out_size)
{
    //read the compressed data
    char* dataReadInCompressed = new char[sizeDataCompressed];
    br = fread(dataReadInCompressed, 1, sizeDataCompressed, fp);
    fclose(fp);

    if (br != sizeDataCompressed) { yError() << "problems reading file!"; delete[] dataReadInCompressed; return false; }

    size_t h = ((size_t*)(dataReadInCompressed))[0]; //byte 0
    size_t w = ((size_t*)(dataReadInCompressed))[1]; //byte 8, because size_t is 8 bytes long
    size_t hds = 2 * sizeof(size_t); //16 bytes

    dest.resize(w, h);
    unsigned char* destbuff = dest.getRawImage();
    //this is the size of the image
    size_t sizeDataUncompressed = dest.getRawImageSize();
    //this is the size of the buffer. Extra space is required for temporary operations (I choose arbitrarily *2)
    size_t sizeDataUncompressedExtra = sizeDataUncompressed * 2;

    char* dataUncompressed = new char[sizeDataUncompressedExtra];

    int z_result = uncompress((Bytef*)dataUncompressed, (uLongf*)&sizeDataUncompressedExtra, (const Bytef*)dataReadInCompressed + hds, sizeDataCompressed - hds);
    switch (z_result)
    {
    case Z_OK:
        break;

    case Z_MEM_ERROR:
        yCError(ZLIBMONITOR, "zlib compression: out of memory");
        delete[] dataUncompressed;
        return false;
        break;

    case Z_BUF_ERROR:
        yCError(ZLIBMONITOR, "zlib compression: output buffer wasn't large enough");
        delete[] dataUncompressed;
        return false;
        break;

    case Z_DATA_ERROR:
        yCError(ZLIBMONITOR, "zlib compression: file contains corrupted data");
        delete[] dataUncompressed;
        return false;
        break;
    }

    //here I am copy only the size of the image, obviously the extra space is not needed anymore.
    for (size_t i = 0; i < sizeDataUncompressed; i++)
    {
        destbuff[i] = dataUncompressed[i];
    }

    delete[] dataUncompressed;
    return true;

}

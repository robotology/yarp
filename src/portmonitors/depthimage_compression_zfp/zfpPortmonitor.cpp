/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "zfpPortmonitor.h"

#include <yarp/os/LogComponent.h>
#include <yarp/sig/Image.h>

#include <cstring>
#include <cmath>
#include <algorithm>

extern "C" {
    #include "zfp.h"
}

using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(ZFPMONITOR,
                   "yarp.carrier.portmonitor.depthimage_compression_zfp",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
}


bool ZfpMonitorObject::create(const yarp::os::Property& options)
{
    shouldCompress = (options.find("sender_side").asBool());
    compressed = nullptr;
    decompressed = nullptr;
    buffer = nullptr;
    sizeToAllocate = 0;
    sizeToAllocateB = 0;
    return true;
}

void ZfpMonitorObject::destroy()
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

bool ZfpMonitorObject::setparam(const yarp::os::Property& params)
{
    return false;
}

bool ZfpMonitorObject::getparam(yarp::os::Property& params)
{
    return false;
}

bool ZfpMonitorObject::accept(yarp::os::Things& thing)
{
    if(shouldCompress){
        ImageOf<PixelFloat>* img = thing.cast_as< ImageOf<PixelFloat> >();
        if(img == nullptr) {
            yCError(ZFPMONITOR, "Expected type ImageOf<PixelFloat> in sender side, but got wrong data type!");
            return false;
        }
    }
    else{
        Bottle* bt= thing.cast_as<Bottle>();
        if(bt == nullptr){
            yCError(ZFPMONITOR, "Expected type Bottle in receiver side, but got wrong data type!");
            return false;
        }

    }


    return true;
}

yarp::os::Things& ZfpMonitorObject::update(yarp::os::Things& thing)
{

   if(shouldCompress) {
        ImageOf<PixelFloat>* img = thing.cast_as< ImageOf<PixelFloat> >();
        // .... buffer, len
        int sizeCompressed;
        compress((float*)img->getRawImage(), compressed, sizeCompressed, img->width(),img->height(),1e-3);
        if(!compressed){
            yCError(ZFPMONITOR, "Failed to compress, exiting...");
            return thing;
        }
        data.clear();
        data.addInt32(img->width());
        data.addInt32(img->height());
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

       if(!decompressed){
           yCError(ZFPMONITOR, "Failed to decompress, exiting...");
           return thing;
       }
       imageOut.resize(width,height);
       memcpy(imageOut.getRawImage(),decompressed,width*height*4);
       th.setPortWriter(&imageOut);

   }

    return th;
}

void ZfpMonitorObject::resizeF(float *&array, int newSize){
    if(newSize>sizeToAllocate){
        sizeToAllocate=newSize;
        free(array);
        array=(float*) malloc(sizeToAllocate);
    }


}
void ZfpMonitorObject::resizeV(void *&array, int newSize){
    if(newSize>sizeToAllocateB){
        sizeToAllocateB=newSize;
        free(array);
        array=(void*) malloc(sizeToAllocateB);
    }


}

int ZfpMonitorObject::compress(float* array, float* &compressed, int &zfpsize, int nx, int ny, float tolerance){
    int status = 0;    /* return value: 0 = success */
    zfp_type type;     /* array scalar type */
    zfp_field* field;  /* array meta data */
    zfp_stream* zfp;   /* compressed stream */
    size_t bufsize;    /* byte size of compressed buffer */
    bitstream* stream; /* bit stream to write to or read from */

    type = zfp_type_float;
    field = zfp_field_2d(array, type, nx, ny);

    /* allocate meta data for a compressed stream */
    zfp = zfp_stream_open(nullptr);

    /* set compression mode and parameters via one of three functions */
    /*  zfp_stream_set_rate(zfp, rate, type, 3, 0); */
    /*  zfp_stream_set_precision(zfp, precision); */
    zfp_stream_set_accuracy(zfp, tolerance);

    /* allocate buffer for compressed data */
    bufsize = zfp_stream_maximum_size(zfp, field);

    resizeV(buffer,bufsize);

    /* associate bit stream with allocated buffer */
    stream = stream_open(buffer, bufsize);
    zfp_stream_set_bit_stream(zfp, stream);
    zfp_stream_rewind(zfp);

    /* compress entire array */
    /* compress array and output compressed stream */
    zfpsize = zfp_compress(zfp, field);
    if (!zfpsize) {
      yCError(ZFPMONITOR, "compression failed");
      status = 1;
    }

    resizeF(compressed,zfpsize);
    memcpy(compressed,(float*) stream_data(zfp->stream),zfpsize);

    /* clean up */
    zfp_field_free(field);
    zfp_stream_close(zfp);
    stream_close(stream);

    return status;
}

int ZfpMonitorObject::decompress(float* array, float* &decompressed, int zfpsize, int nx, int ny, float tolerance){
    int status = 0;    /* return value: 0 = success */
    zfp_type type;     /* array scalar type */
    zfp_field* field;  /* array meta data */
    zfp_stream* zfp;   /* compressed stream */
    size_t bufsize;    /* byte size of compressed buffer */
    bitstream* stream; /* bit stream to write to or read from */

    type = zfp_type_float;
    resizeF(decompressed,nx*ny*sizeof(float));
    field = zfp_field_2d(decompressed, type, nx, ny);

    /* allocate meta data for a compressed stream */
    zfp = zfp_stream_open(nullptr);

    /* set compression mode and parameters via one of three functions */
    /*  zfp_stream_set_rate(zfp, rate, type, 3, 0); */
    /*  zfp_stream_set_precision(zfp, precision, type); */
    zfp_stream_set_accuracy(zfp, tolerance);

    /* allocate buffer for compressed data */
    bufsize = zfp_stream_maximum_size(zfp, field);
    resizeV(buffer,bufsize);
    memcpy(buffer,array,zfpsize);

    /* associate bit stream with allocated buffer */
    stream = stream_open(buffer, zfpsize);
    zfp_stream_set_bit_stream(zfp, stream);
    zfp_stream_rewind(zfp);

    /* read compressed stream and decompress array */
    if (!zfp_decompress(zfp, field)) {
      yCError(ZFPMONITOR, "decompression failed");
      status = 1;
    }

    /* clean up */
    zfp_field_free(field);
    zfp_stream_close(zfp);
    stream_close(stream);

    return status;

}

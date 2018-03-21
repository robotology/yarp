/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
 * Authors: Nicolò Genesio
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <yarp/sig/Image.h>
#include <yarp/os/LogStream.h>
extern "C" {
    #include "zfp.h"
}
#include "zfpPortmonitor.h"

using namespace yarp::os;
using namespace yarp::sig;


bool ZfpMonitorObject::create(const yarp::os::Property& options)
{
    shouldCompress = (options.find("sender_side").asBool());
    compressed=NULL;
    decompressed=NULL;
    buffer=NULL;
    sizeToAllocate=0;
    sizeToAllocateB=0;
    return true;
}

void ZfpMonitorObject::destroy(void)
{
    if(compressed){
        free(compressed);
        compressed=NULL;
    }

    if(buffer){
        free(buffer);
        buffer=NULL;
    }

    if(decompressed){
        free(decompressed);
        decompressed=NULL;
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
        if(img == NULL) {
            yError()<<"ZfpMonitorObject: expected type ImageOf<PixelFloat> in sender side, but got wrong data type!";
            return false;
        }
    }
    else{
        Bottle* bt= thing.cast_as<Bottle>();
        if(bt==NULL){
            yError()<<"ZfpMonitorObject: expected type Bottle in receiver side, but got wrong data type!";
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
            yError()<<"ZfpMonitorObject:Failed to compress, exiting...";
            return thing;
        }
        data.clear();
        data.addInt(img->width());
        data.addInt(img->height());
        data.addInt(sizeCompressed);
        Value v(compressed, sizeCompressed);
        data.add(v);
        th.setPortWriter(&data);
   }
   else
   {

       Bottle* compressedbt= thing.cast_as<Bottle>();

       int width=compressedbt->get(0).asInt();
       int height=compressedbt->get(1).asInt();
       int sizeCompressed=compressedbt->get(2).asInt();
       // cast thing to compressed.
       decompress((float*)compressedbt->get(3).asBlob(), decompressed, sizeCompressed, width, height,1e-3);

       if(!decompressed){
           yError()<<"ZfpMonitorObject:Failed to decompress, exiting...";
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
    zfp = zfp_stream_open(NULL);

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
      fprintf(stderr, "compression failed\n");
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
    zfp = zfp_stream_open(NULL);

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
      fprintf(stderr, "decompression failed\n");
      status = 1;
    }

    /* clean up */
    zfp_field_free(field);
    zfp_stream_close(zfp);
    stream_close(stream);

    return status;

}


/*
 * Copyright (C) 2016 iCub Facility
 * Authors: Nicolo' Genesio
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <yarp/sig/Image.h>
#include <yarp/os/LogStream.h>
#include "zfp.h"

#include "zfpPortmonitor.h"

using namespace yarp::os;
using namespace yarp::sig;


bool ZfpMonitorObject::create(const yarp::os::Property& options)
{
   shouldCompress = (options.find("sender_side").asBool());
   compressed=NULL;
   decompressed=NULL;
   //yDebug()<< ((shouldCompress) ? "compressing..." : "decompressing...");
   return true;
}

void ZfpMonitorObject::destroy(void)
{
    //yDebug()<<"destroyed";
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
       if(compressed){
           free(compressed);
           compressed=NULL;
       }
        ImageOf<PixelFloat>* img = thing.cast_as< ImageOf<PixelFloat> >();
        // .... buffer, len
        int sizeCompressed;
        compress((float*)img->getRawImage(), compressed, sizeCompressed, img->width(),img->height(),1e-3);
        if(!compressed){
            yError()<<"Failed to compress, exiting...";
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
       if(decompressed){
           free(decompressed);
           decompressed=NULL;
       }
       Bottle* compressedbt= thing.cast_as<Bottle>();

       int width=compressedbt->get(0).asInt();
       int height=compressedbt->get(1).asInt();
       int sizeCompressed=compressedbt->get(2).asInt();
       compressed=(float*)compressedbt->get(3).asBlob();
       //yDebug()<<"W"<<width<<"H"<<height<<"sizecomp"<<sizeCompressed;
       // cast thing to compressed.
       decompress(compressed, decompressed, sizeCompressed, width, height,1e-3);

       if(!decompressed){
           yError()<<"Failed to decompress, exiting...";
           return thing;
       }
       imageOut.resize(width,height);
       memcpy(imageOut.getRawImage(),decompressed,width*height*4);
       th.setPortWriter(&imageOut);

   }


    //bt.clear();

    //th.setPortWriter(&bt);
    return th;
}

int ZfpMonitorObject::compress(float* array, float* &compressed, int &zfpsize, int nx, int ny, float tolerance){
    int status = 0;    /* return value: 0 = success */
    zfp_type type;     /* array scalar type */
    zfp_field* field;  /* array meta data */
    zfp_stream* zfp;   /* compressed stream */
    void* buffer;      /* storage for compressed stream */
    size_t bufsize;    /* byte size of compressed buffer */
    bitstream* stream; /* bit stream to write to or read from */

    type = zfp_type_float;
    field = zfp_field_2d(array, type, nx, ny);

    /* allocate meta data for a compressed stream */
    zfp = zfp_stream_open(NULL);

    /* set compression mode and parameters via one of three functions */
    /*  zfp_stream_set_rate(zfp, rate, type, 3, 0); */
    /*  zfp_stream_set_precision(zfp, precision, type); */
    zfp_stream_set_accuracy(zfp, tolerance, type);

    /* allocate buffer for compressed data */
    bufsize = zfp_stream_maximum_size(zfp, field);
    buffer = malloc(bufsize);

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
    //else
        //yInfo()<<"compression successful, ratio of compression:"<<(nx*ny*4.0)/(zfpsize)<<":1"<<"orgSize="
              //<<nx*ny*4.0<<"compressedSize="<<zfpsize;//4 -> float


    compressed = (float*) malloc(zfpsize);
    memcpy(compressed,(float*) stream_data(zfp->stream),zfpsize);

    /* clean up */
    zfp_field_free(field);
    zfp_stream_close(zfp);
    stream_close(stream);
    free(buffer);

    return status;
}

int ZfpMonitorObject::decompress(float* array, float* &decompressed, int zfpsize, int nx, int ny, float tolerance){
    int status = 0;    /* return value: 0 = success */
    zfp_type type;     /* array scalar type */
    zfp_field* field;  /* array meta data */
    zfp_stream* zfp;   /* compressed stream */
    void* buffer;      /* storage for compressed stream */
    size_t bufsize;    /* byte size of compressed buffer */
    bitstream* stream; /* bit stream to write to or read from */

    type = zfp_type_float;
    decompressed = (float*) malloc(nx*ny*sizeof(float));
    field = zfp_field_2d(decompressed, type, nx, ny);

    /* allocate meta data for a compressed stream */
    zfp = zfp_stream_open(NULL);

    /* set compression mode and parameters via one of three functions */
    /*  zfp_stream_set_rate(zfp, rate, type, 3, 0); */
    /*  zfp_stream_set_precision(zfp, precision, type); */
    zfp_stream_set_accuracy(zfp, tolerance, type);

    /* allocate buffer for compressed data */
    bufsize = zfp_stream_maximum_size(zfp, field);
    buffer = malloc(bufsize);
    memcpy(buffer,array,zfpsize);


//    /* associate bit stream with allocated buffer */
    stream = stream_open(buffer, zfpsize);
    zfp_stream_set_bit_stream(zfp, stream);
    zfp_stream_rewind(zfp);

    /* compress or decompress entire array */
    /* read compressed stream and decompress array */
    //zfpsize = fread(buffer, 1, bufsize, stdin);
    //devo metterci il mio puntatore al dato compresso.
    if (!zfp_decompress(zfp, field)) {
      fprintf(stderr, "decompression failed\n");
      status = 1;
    }
    //else
        //yInfo()<<"Decompression successful";
    //yDebug()<<"bufsize"<<bufsize<<"zfpsize"<<zfpsize;
    //std::cout<<"test decompressed data "<<((float*) field->data)[76799]<<std::endl; //OK
//    ((float*) field->data)[i + nx * (j)]
    //nx*ny*sizeof(float)
    //memcpy(decompressed,(float*) field->data,nx*ny*sizeof(float));


    /* clean up */
    zfp_field_free(field);
    zfp_stream_close(zfp);
    stream_close(stream);
    free(buffer);

    return status;

}


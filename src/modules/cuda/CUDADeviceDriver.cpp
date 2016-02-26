/*
 * Copyright (C) 2007 Giacomo Spigler
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <cuda.h>
#include <cutil.h>

#include "CUDADeviceDriver.h"
#include "FunctionClass.h"


CUdevice cuDevice;
CUcontext cuContext;



bool CUDAGPU::open(int w, int h, int bytespp, int elemtype) {
    this->w=w;
    this->h=h;

    this->changebpp(bytespp);
    this->changetype(elemtype);

    if(this->w>0 && this->h>0) {
      #if CUDA_VERSION>=2000
        CUT_DEVICE_INIT_DRV(cuDevice, 0, NULL);
      #else
        CUT_DEVICE_INIT_DRV(cuDevice);
      #endif

      CUresult status = cuCtxCreate(&cuContext, 0, cuDevice);
      if(CUDA_SUCCESS != status)
        return false;


      size=w*h*bytespp;

      CUdeviceptr tex_data = (CUdeviceptr)NULL;
      CU_SAFE_CALL( cuMemAlloc(&tex_data, size) );

      CUdeviceptr tex2_data = (CUdeviceptr)NULL;
      CU_SAFE_CALL( cuMemAlloc(&tex2_data, size) );

      CUdeviceptr oTex_data = (CUdeviceptr)NULL;
      CU_SAFE_CALL( cuMemAlloc(&oTex_data, size) );

      tex=(unsigned int)tex_data;
      tex2=(unsigned int)tex2_data;
      oTex=(unsigned int)oTex_data;
    }

    return w>0 && h>0;
}


bool CUDAGPU::close() {
    CU_SAFE_CALL( cuMemFree((CUdeviceptr)tex) );
    CU_SAFE_CALL( cuMemFree((CUdeviceptr)tex2) );
    CU_SAFE_CALL( cuMemFree((CUdeviceptr)oTex) );

    CU_SAFE_CALL_NO_SYNC( cuCtxDetach(cuContext) );

    return true;
}


bool CUDAGPU::resize(int width, int height) {
    this->w=width;
    this->h=height;

    if(this->w>0 && this->h>0) {
      CU_SAFE_CALL( cuMemFree((CUdeviceptr)tex) );
      CU_SAFE_CALL( cuMemFree((CUdeviceptr)tex2) );
      CU_SAFE_CALL( cuMemFree((CUdeviceptr)oTex) );


      CUdeviceptr tex_data = (CUdeviceptr)NULL;
      CU_SAFE_CALL( cuMemAlloc(&tex_data, size) );

      CUdeviceptr tex2_data = (CUdeviceptr)NULL;
      CU_SAFE_CALL( cuMemAlloc(&tex2_data, size) );

      CUdeviceptr oTex_data = (CUdeviceptr)NULL;
      CU_SAFE_CALL( cuMemAlloc(&oTex_data, size) );

      tex=(unsigned int)tex_data;
      tex2=(unsigned int)tex2_data;
      oTex=(unsigned int)oTex_data;
    }

    return w>0 && h>0;
}


int CUDAGPU::load(char *name) {
    return (int)new CUDAFunction(name);
}


void CUDAGPU::setargument(int prg, int val) {
    CUDAFunction *cf=(CUDAFunction *)prg;

    CUfunction func=cf->func;

    CU_SAFE_CALL(cuParamSeti(func, cf->offset, val));
    cf->offset += sizeof(val);

    //CU_SAFE_CALL(cuParamSetSize(func, offset));

    //CU_SAFE_CALL(cuParamSetTexRef(transform, CU_PARAM_TR_DEFAULT, cu_texref));
}


void CUDAGPU::setargument(int prg, float val) {
    CUDAFunction *cf=(CUDAFunction *)prg;

    CUfunction func=cf->func;

    CU_SAFE_CALL(cuParamSetf(func, cf->offset, val));
    cf->offset += sizeof(val);

    //CU_SAFE_CALL(cuParamSetSize(func, offset));

    //CU_SAFE_CALL(cuParamSetTexRef(transform, CU_PARAM_TR_DEFAULT, cu_texref));
}


//Actually in and out type's is the same!
void CUDAGPU::execute(int prg, unsigned char *in, unsigned char *out) {
    CUDAFunction *cf=(CUDAFunction *)prg;


    cuMemcpyHtoD((CUdeviceptr)tex, in, size);

    setargument(prg, (int)w);
    setargument(prg, (int)h);
    setargument(prg, (int)(CUdeviceptr)tex);
    setargument(prg, (int)(CUdeviceptr)oTex);

    CU_SAFE_CALL(cuParamSetSize(cf->func, cf->offset));

    CU_SAFE_CALL(cuFuncSetBlockShape(cf->func, CUDA_NUMTHREADS, 1, 1));


////    CU_SAFE_CALL(cuLaunch(cf->func));
//warmup?
//    CU_SAFE_CALL(cuLaunchGrid(cf->func, 96, 1));

//    CU_SAFE_CALL(cuCtxSynchronize());


//
//    unsigned int timer = 0;
//    CUT_SAFE_CALL( cutCreateTimer( &timer));
//    CUT_SAFE_CALL( cutStartTimer( timer));
//

    CU_SAFE_CALL(cuLaunchGrid(cf->func, CUDA_NUMTHREADBLOCKS, 1));

    CU_SAFE_CALL(cuCtxSynchronize());

//
//    CUT_SAFE_CALL(cutStopTimer( timer));
//    printf("Processing time: %f (ms)\n", cutGetTimerValue(timer));
//    CUT_SAFE_CALL(cutDeleteTimer(timer));
//


    CU_SAFE_CALL(cuMemcpyDtoH(out, (CUdeviceptr)oTex, size));
}


void CUDAGPU::execute(int prg, unsigned char *in, unsigned char *in2, unsigned char *out) {
    CUDAFunction *cf=(CUDAFunction *)prg;


    cuMemcpyHtoD((CUdeviceptr)tex, in, size);
    cuMemcpyHtoD((CUdeviceptr)tex2, in2, size);

    setargument(prg, (int)w);
    setargument(prg, (int)h);
    setargument(prg, (int)(CUdeviceptr)tex);
    setargument(prg, (int)(CUdeviceptr)tex2);
    setargument(prg, (int)(CUdeviceptr)oTex);

    CU_SAFE_CALL(cuParamSetSize(cf->func, cf->offset));

    CU_SAFE_CALL(cuFuncSetBlockShape(cf->func, CUDA_NUMTHREADS, 1, 1));


//warmup
//    CU_SAFE_CALL(cuLaunchGrid(cf->func, 96, 1));

//    CU_SAFE_CALL(cuCtxSynchronize());

//
//    unsigned int timer = 0;
//    CUT_SAFE_CALL( cutCreateTimer( &timer));
//    CUT_SAFE_CALL( cutStartTimer( timer));
//

    CU_SAFE_CALL(cuLaunchGrid(cf->func, CUDA_NUMTHREADBLOCKS, 1));

    CU_SAFE_CALL(cuCtxSynchronize());

//
//    CUT_SAFE_CALL(cutStopTimer( timer));
//    printf("Processing time: %f (ms)\n", cutGetTimerValue(timer));
//    CUT_SAFE_CALL(cutDeleteTimer(timer));
//


    CU_SAFE_CALL(cuMemcpyDtoH(out, (CUdeviceptr)oTex, size));
}


void CUDAGPU::execute(int prg, yarp::sig::Image *in, yarp::sig::Image *out) {
    unsigned char *input=in->getRawImage();
    unsigned char *output=out->getRawImage();

    this->execute(prg, input, output);
}


void CUDAGPU::execute(int prg, yarp::sig::Image *in, yarp::sig::Image *in2, yarp::sig::Image *out) {
    unsigned char *input=in->getRawImage();
    unsigned char *input2=in2->getRawImage();
    unsigned char *output=out->getRawImage();

    this->execute(prg, input, input2, output);
}





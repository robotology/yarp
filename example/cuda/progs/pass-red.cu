/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2007 Giacomo Spigler
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

extern "C" {

  __global__ void FragmentProgram(int w, int h, unsigned char *in, unsigned char *out) {
    int i=0;

    for(i=threadIdx.x+blockIdx.x*blockDim.x; i<w*h; i+=blockDim.x*gridDim.x) {
      if(in[i*3]>=200) {
        //out[i*3]=255;
        //out[i*3+1]=255;
        //out[i*3+2]=255;
      } else {
        out[i*3]=0;
        out[i*3+1]=0;
        out[i*3+2]=0;
      }
    }
  }

}



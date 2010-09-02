/*
 * Copyright (C) 2007 Giacomo Spigler
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

extern "C" {

  __global__ void FragmentProgram(int w, int h, unsigned char *in, unsigned char *out) {
    int i=0;

    for(i=threadIdx.x+blockIdx.x*blockDim.x; i<w*h; i+=blockDim.x*gridDim.x) {
      out[i*3]=in[i*3+2];
      out[i*3+1]=in[i*3+1];
      out[i*3+2]=in[i*3];
    }
  }

}


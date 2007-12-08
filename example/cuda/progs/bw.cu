#define NUMTHREADS 384

__global__ void FragmentProgram(int w, int h, unsigned char *in, unsigned char *out) {
  int i=0;

  for(i=threadIdx.x+blockIdx.x*ATHREAD_COUNT; i<w*h; i+=ATHREAD_COUNT*BLOCKS) {
    out[i*3]=(in[i*3]+in[i*3+1]+in[i*3+2])/3;
    out[i*3+1]=out[i*3];
    out[i*3+2]=out[i*3];
  }
}



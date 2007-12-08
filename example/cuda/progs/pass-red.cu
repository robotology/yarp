#define ATHREAD_COUNT 32
#define BLOCKS 96

__global__ void FragmentProgram(int w, int h, unsigned char *in, unsigned char *out) {
  int i=0;

  for(i=threadIdx.x+blockIdx.x*ATHREAD_COUNT; i<w*h; i+=ATHREAD_COUNT*BLOCKS) {
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



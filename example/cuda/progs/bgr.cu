#define NUMTHREADS 384

__global__ void FragmentProgram(int size, unsigned char *in, unsigned char *out) {
  int i=0;
  size=size/3;

  for(i=threadIdx.x; i<size; i=i+NUMTHREADS) {
    out[i*3]=in[i*3+2];
    out[i*3+1]=in[i*3+1];
    out[i*3+2]=in[i*3];
  }
}



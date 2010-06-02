#include "MjpegStream.h"

using namespace yarp::os::impl;
using namespace std;

int MjpegStream::read(const Bytes& b) {
  int result = sis.read(b);
  if (result>0) {
    printf("RETURNING %d bytes\n", result);
    return result;
  }
  if (result==0) {
    printf("Reading...\n");
    if (sender) {
      //
    } else {
      //
    }
    if (firstRound) {
      if (sender) {
	//
      } else {
	//
      }
      firstRound = false;
    }
  }
  printf("RETURNING %d bytes\n", result);
  return (result>0)?result:-1;
}


void MjpegStream::write(const Bytes& b) {
  delegate->getOutputStream().write(b);
}

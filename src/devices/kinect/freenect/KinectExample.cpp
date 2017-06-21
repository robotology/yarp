/*
 * Copyright (C) 2010 Philipp Robbel
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 * If you want to access the Kinect as a user other than root, use some
 * variation of the included udev rule file.
 */

#include <iostream>
#include <sstream>
using namespace std;

#include "KinectDeviceDriver.h"
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>


void spin(KinectDeviceDriver & h, Searchable & s) {
  while (true) {
    if(h.open(s) == true) {
      while(true) {
        if(!h.hasDepth() || !h.hasRgb()) {
          SystemClock::delaySystem(0.01);
          continue;
        }

        PortablePair<ImageOf<PixelMono16>, ImageOf<PixelRgb> > pair;
        // XXX question: are the dtors of HEAD,BODY called when this goes out of scope (yes)?
        bool res = h.getImagePair(pair);
        if(res == false)
          break;

        // testing..
        IplImage *cvImage = (IplImage*)pair.body.getIplImage();
        printf("Showing OpenCV/IPL image\n");
        cvNamedWindow("test",1);
        cvShowImage("test",cvImage);
        cvWaitKey(1000);
      }
    }
    else
      usleep(1000000);
  }

  // stopping should be fine even if not running
  h.close();
}

int main() {
  // Initialize device string
  ostringstream os;
  os << "(device kinect)";
  cout << "[DEBUG]: Device string: " << os.str() << endl;

  Property p;
  p.fromString(os.str().c_str());

  KinectDeviceDriver k;
  spin(k, p);

  return EXIT_SUCCESS;
}

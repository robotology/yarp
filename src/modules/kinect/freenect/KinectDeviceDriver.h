/*
 * Copyright (C) 2010 Philipp Robbel
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 * Implements basic Kinect driver.
 *
 * NOTE: In its current form, the camera needs a few seconds of startup time
 * before it works. If launched too early, garbled images may result.
 */

#ifndef _KINECTDEVICEDRIVER_H_
#define _KINECTDEVICEDRIVER_H_

#include <memory.h>
#include <libusb.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Image.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>

#include "libfreenect.h"

namespace yarp {
  namespace dev {
    class KinectDeviceDriverSettings;
    class KinectDeviceDriver;
  }
}


/**
 * For documentation of settings, see above.
 */
class yarp::dev::KinectDeviceDriverSettings { };

class yarp::dev::KinectDeviceDriver : public DeviceDriver {
public:
  KinectDeviceDriver() :
    width_(640),
    height_(480),
    maxRange_(5.), // XXX not sure
    horizontalFOV_(57.),
    verticalFOV_(43.) {
      rgbBuf_ = new uint8_t[width_*height_*3];
      depthBuf_ = new uint16_t[width_*height_];
      depthSent_ = true; // no new data available yet
      rgbSent_ = true;
    }

  virtual ~KinectDeviceDriver() {
    close();
    // clear up internal memory buffers
    if(rgbBuf_) { delete [] rgbBuf_; rgbBuf_ = 0; }
    if(depthBuf_) { delete [] depthBuf_; depthBuf_ = 0; }
  }

  virtual bool open(yarp::os::Searchable& config) {
    return open(cfg_);
  }

  /**
   * Configure the device.
   * @return true on success
   */
  bool open(const KinectDeviceDriverSettings & cfg);

  virtual bool close();

  /**
   * Get an image pair (depth and intensity images) from the kinect camera.
   * @return true/false upon success/failure
   */
  bool getImagePair(yarp::os::PortablePair<yarp::sig::ImageOf<yarp::sig::PixelMono16>, yarp::sig::ImageOf<yarp::sig::PixelRgb> > & pair) {
    pair.head.resize(width_,height_);
    pair.body.resize(width_,height_);

    depthMutex_.wait();
    // must transfer row by row, since YARP may use padding
    for (int i=0; i<height_; i++) {
      // depth image (1 layer)
      memcpy((unsigned char *)(&pair.head.pixel(0,i)),&depthBuf_[width_*i],width_*sizeof(uint16_t));
    }
    depthSent_ = true;
    depthMutex_.post();

    rgbMutex_.wait();
    for (int i=0; i<height_; i++) {
      // rgb image (3 layers), requires PixelRgb return value
      memcpy((unsigned char *)(&pair.body.pixel(0,i)),&rgbBuf_[i*width_*3],width_*3);
    }
    rgbSent_ = true;
    rgbMutex_.post();

    return true;
  }

  /**
   * Check whether new depth or rgb data have been retrieved from device.
   * Follow up with call to getImagePair() to obtain the actual images.
   */
  bool hasDepth() {
    return !depthSent_;
  }

  bool hasRgb() {
    return !rgbSent_;
  }

  /**
   * Return the height of each frame.
   * @return image height
   */
  int height() const {
    return height_;
  }

  /**
   * Return the width of each frame.
   * @return image width
   */
  int width() const {
    return width_;
  }

  /**
   * Return the maximum range of the sensor
   * @return sensor range
   */
  int maxRange() const {
    return maxRange_;
  }

  /**
   * Depth and rgb callbacks fill internal image buffers
   */
  void depthImgCb(uint16_t *buf) {
    depthMutex_.wait();
    depthSent_ = false; // new depth data available
    memcpy(depthBuf_, buf, width_*height_*sizeof(uint16_t));
    depthMutex_.post();
  }

  void rgbImgCb(uint8_t *buf) {
    rgbMutex_.wait();
    rgbSent_ = false; // new rgb data available
    memcpy(rgbBuf_, buf, width_*height_*3);
    rgbMutex_.post();
  }

private:
  //! driver settings (empty for now)
  yarp::dev::KinectDeviceDriverSettings cfg_;

  //! fixed device properties
  const int width_;
  const int height_;
  const double maxRange_;
  const double horizontalFOV_;
  const double verticalFOV_;

  //! depth and rgb buffers from the kinect camera
  yarp::os::Semaphore depthMutex_;
  uint16_t *depthBuf_;
  yarp::os::Semaphore rgbMutex_;
  uint8_t *rgbBuf_;

  bool depthSent_;
  bool rgbSent_;

  /**
   * Handle interaction with USB driver and fill internal image buffers
   */
  class USBThread : public yarp::os::Thread {
  public:
    virtual void run() {
      // XXX Time::delay may interfere with USB read?!
      while(!isStopping() && libusb_handle_events(NULL) == 0);
    }
  } usbSpinThread_;
};

#endif

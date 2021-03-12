fake_laser_obstacle {#master}
-------------------------

## Important Changes

### Devices

#### fakeFrameGrabber
* added rpc port, to change its mode at runtime. It is also possibile to load a new background image at runtime. Rpc command 'help' will display available commands.

### yarp::sig

#### yarp::sig::ImageFile
* `yarp::sig::ImageFile.cpp` Added support to read .jpg files
* Added the following methods:
  ```
  bool ImageReadRGB_JPG(ImageOf<PixelRgb>& img, const char* filename);
  bool ImageReadBGR_JPG(ImageOf<PixelBgr>& img, const char* filename);
  bool ImageReadMono_JPG(ImageOf<PixelMono>& img, const char* filename);
  ```
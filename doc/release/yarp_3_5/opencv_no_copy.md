opencv_no_copy {#yarp_3_5}
-----------

### Devices

#### `opencv_grabber`

* Efficiency has been improved as copies are (in most cases) no longer
  performed in order to convert from OpenCV frames to YARP image structures.
* Options `--flip_x`, `--flip_y` and `--transpose` are now also available
  when reading from file (`--movie`).

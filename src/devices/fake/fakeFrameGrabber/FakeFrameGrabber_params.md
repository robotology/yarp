  * |      |  width             | int     | -             |   320         | No    | desired width of test image                       |  |
  * |      |  height            | int     | -             |   240         | No    | desired height of test image                      |  |
  * |      |  horizontalFov     | double  | -             |   1.0         | No    | desired horizontal fov of test image              |  |
  * |      |  verticalFov       | double  | -             |   2.0         | No    | desired horizontal fov of test image              |  |
  * |      |  fakeFrameGrabber_rpc_port   | string  | -  | /fakeFrameGrabber/rpc    | No    | rpc port for the fakeFrameGrabber                       |  |
  * |      |  mirror            | bool  | -              |   false      | No    | mirror height of test image                      |  |
  * |      |  syncro            | bool  | -              |   false      | No    | synchronize producer and consumer, so that all images are used once and only once              |  |
  * |      |  topIsLow          | bool  | -              |   true       | No    | explicitly set the topIsLow field in the images              |  |
  * |      |  physFocalLength  | double  | -             |   3.0         | No    | Physical focal length                              |  |
  * |      |  focalLengthX     | double  | -             |   4.0         | No    | Horizontal component of the focal length           |  |
  * |      |  focalLengthY     | double  | -             |   5.0         | No    | Vertical component of the focal length             |  |
  * |      |  principalPointX  | double  | -             |   6.0         | No    | X coordinate of the principal point                |  |
  * |      |  principalPointY  | double  | -             |   7.0         | No    | Y coordinate of the principal point                |  |
  * |      |  distortionModel | string  | -             |   FishEye      | No    | Reference to group of parameters describing the distortion model of the camera     |  |
  * |      |  k1              | double  | -             |   8.0          | No    | Radial distortion coefficient of the lens(fake                              |  |
  * |      |  k2              | double  | -             |   9.0          | No    | Radial distortion coefficient of the lens(fake)                             |  |
  * |      |  k3              | double  | -             |   10.0         | No    | Radial distortion coefficient of the lens(fake)                             |  |
  * |      |  t1              | double  | -             |   11.0         | No    | Tangential distortion of the lens(fake)                                     |  |
  * |      |  t2              | double  | -             |   12.0         | No    | Tangential distortion of the lens(fake)                                     |  |
  * |      |  freq            | double  | -             |   0            | No    | rate of test images in Hz                                     |  |
  * |      |  period          | double  | -             |   0            | No    | period of test images in seconds                              |  |
  * |      |  mode            | string  | -             |   [line]       | No    | bouncy [ball], scrolly [line], grid [grid], grid multisize [size], random [rand], none [none], time test[time]       |  |
  * |      |  src             | string  | -             |  -             | No    | background image to use, if any       | e.g. test.ppm |
  * |      |  add_timestamp   | bool    | -             |  false       | No    | should write the timestamp in the first bytes of the image       |  |
  * |      |  add_noise       | bool    | -             |  false       | No    | should add noise to the image (uses snr parameter)           |  |
  * |      |  bayer           | bool    | -             |  false       | No    | should emit bayer test image       |  |
  * |      |  mono            | bool    | -             |  false       | No    | should emit a monochrome image     |  |
  * |      |  snr             | double  | -             |  0.5         | No    | Signal noise ratio ([0.0-1.0]       |  |
  * |      |  rectificationMatrix     | vector<double>  | -   |  (1.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0) | No    | Matrix that describes the lens' distortion    |  |

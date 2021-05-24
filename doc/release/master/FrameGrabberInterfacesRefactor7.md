FrameGrabberInterfacesRefactor7 {#master}
-------------------------------

## Libraries

### `dev`

* The `IFrameGrabberImage` and `IFrameGrabberImageRaw` interface are now aliases
  for `IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>` and
  `IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>`.
  It is also possible to implement the same interface for different pixel types
  or for `FlexImage`.

fix_framegrabbercontrols_setfeature_ack {#yarp_3_12}
-----------------------

## Devices

### `frameGrabber_nwc_yarp`, `RGBDSensorClient`

* Fixed `FrameGrabberControls_Responder`/`Forwarder` silently reporting RPC
  transport success instead of the underlying device operation result for
  `setFeature`, `setActive`, `setMode`, and `setOnePush`. These calls now
  correctly return `false` when the device rejects the request, matching
  the existing `RgbVisualParams`/`DepthVisualParams` behavior (#3352).

ILocalization2D_thrift {#master}
-----------

Important Changes
-----------------

### devices

*  Added new thrift interface `ILocalization2DMsgs`.
*  Added new device `localization2D_nwc_yarp` which can connect only to `localization2D_nws_yarp` (and not `Localization2DServer`).
   This because the communication between `localization2D_nwc_yarp` and `localization2D_nws_yarp` uses the new thrift
   interface `ILocalization2DMsgs`, while `Localization2DClient` and `Localization2DServer` communicate through an hand-written protocol
   based on yarp vocabs.
*  Added a warning to device `localization2DClient`. The device will be deprecated in the next release. 
